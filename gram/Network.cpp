/* Network.cpp
 *
 * Copyright (C) 2009-2012,2013,2014 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2009/02/27 created
 * pb 2009/03/05 setClamping
 * pb 2009/05/14 zeroActivities, normalizeActivities
 * pb 2009/06/11 connection plasticities
 * pb 2011/03/29 C++
 * pb 2012/03/18 more weight update rules: instar, outstar, inoutstar
 * pb 2012/04/19 more activation clipping rules: linear
 * pb 2012/06/02 activation spreading rules: sudden, gradual
 */

#include "Network.h"

#include "oo_DESTROY.h"
#include "Network_def.h"
#include "oo_COPY.h"
#include "Network_def.h"
#include "oo_EQUAL.h"
#include "Network_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Network_def.h"
#include "oo_WRITE_TEXT.h"
#include "Network_def.h"
#include "oo_READ_TEXT.h"
#include "Network_def.h"
#include "oo_WRITE_BINARY.h"
#include "Network_def.h"
#include "oo_READ_BINARY.h"
#include "Network_def.h"
#include "oo_DESCRIPTION.h"
#include "Network_def.h"

#include "enums_getText.h"
#include "Network_enums.h"
#include "enums_getValue.h"
#include "Network_enums.h"

void structNetwork :: v_info ()
{
	structData :: v_info ();
	MelderInfo_writeLine (L"Spreading rate: ", Melder_double (our spreadingRate));
	MelderInfo_writeLine (L"Activity clipping rule: ", kNetwork_activityClippingRule_getText (our activityClippingRule));
	MelderInfo_writeLine (L"Minimum activity: ", Melder_double (our minimumActivity));
	MelderInfo_writeLine (L"Maximum activity: ", Melder_double (our maximumActivity));
	MelderInfo_writeLine (L"Activity leak: ", Melder_double (our activityLeak));
	MelderInfo_writeLine (L"Learning rate: ", Melder_double (our learningRate));
	MelderInfo_writeLine (L"Minimum weight: ", Melder_double (our minimumWeight));
	MelderInfo_writeLine (L"Maximum weight: ", Melder_double (our maximumWeight));
	MelderInfo_writeLine (L"Weight leak: ", Melder_double (our weightLeak));
	MelderInfo_writeLine (L"Number of nodes: ", Melder_integer (our numberOfNodes));
	MelderInfo_writeLine (L"Number of connections: ", Melder_integer (our numberOfConnections));
}

Thing_implement (Network, Data, 6);

void Network_init (Network me, double spreadingRate, enum kNetwork_activityClippingRule activityClippingRule,
	double minimumActivity, double maximumActivity, double activityLeak,
	double learningRate, double minimumWeight, double maximumWeight, double weightLeak,
	double xmin, double xmax, double ymin, double ymax, long numberOfNodes, long numberOfConnections)
{
	my spreadingRate = spreadingRate;
	my activityClippingRule = activityClippingRule;
	my minimumActivity = minimumActivity;
	my maximumActivity = maximumActivity;
	my activityLeak = activityLeak;
	my learningRate = learningRate;
	my minimumWeight = minimumWeight;
	my maximumWeight = maximumWeight;
	my weightLeak = weightLeak;
	my instar = 0.0;
	my outstar = 0.0;
	my xmin = xmin;
	my xmax = xmax;
	my ymin = ymin;
	my ymax = ymax;
	my numberOfNodes = numberOfNodes;
	my nodes = NUMvector <structNetworkNode> (1, numberOfNodes);
	my numberOfConnections = numberOfConnections;
	my connections = NUMvector <structNetworkConnection> (1, numberOfConnections);
}

Network Network_create (double spreadingRate, enum kNetwork_activityClippingRule activityClippingRule,
	double minimumActivity, double maximumActivity, double activityLeak,
	double learningRate, double minimumWeight, double maximumWeight, double weightLeak,
	double xmin, double xmax, double ymin, double ymax, long numberOfNodes, long numberOfConnections)
{
	try {
		autoNetwork me = Thing_new (Network);
		Network_init (me.peek(), spreadingRate, activityClippingRule, minimumActivity, maximumActivity, activityLeak,
			learningRate, minimumWeight, maximumWeight, weightLeak,
			xmin, xmax, ymin, ymax, numberOfNodes, numberOfConnections);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Network not created.");
	}
}

double Network_getActivity (Network me, long nodeNumber) {
	try {
		if (nodeNumber <= 0 || nodeNumber > my numberOfNodes)
			Melder_throw (me, ": node number (", nodeNumber, ") out of the range 1..", my numberOfNodes, ".");
		return my nodes [nodeNumber]. activity;
	} catch (MelderError) {
		Melder_throw (me, ": activity not gotten.");
	}
}

void Network_setActivity (Network me, long nodeNumber, double activity) {
	try {
		if (nodeNumber <= 0 || nodeNumber > my numberOfNodes)
			Melder_throw (me, ": node number (", nodeNumber, ") out of the range 1..", my numberOfNodes, ".");
		my nodes [nodeNumber]. activity = my nodes [nodeNumber]. excitation = activity;
	} catch (MelderError) {
		Melder_throw (me, ": activity not set.");
	}
}

double Network_getWeight (Network me, long connectionNumber) {
	try {
		if (connectionNumber <= 0 || connectionNumber > my numberOfConnections)
			Melder_throw (me, ": connection number (", connectionNumber, ") out of the range 1..", my numberOfConnections, ".");
		return my connections [connectionNumber]. weight;
	} catch (MelderError) {
		Melder_throw (me, ": weight not gotten.");
	}
}

void Network_setWeight (Network me, long connectionNumber, double weight) {
	try {
		if (connectionNumber <= 0 || connectionNumber > my numberOfConnections)
			Melder_throw (me, ": connection number (", connectionNumber, ") out of the range 1..", my numberOfConnections, ".");
		my connections [connectionNumber]. weight = weight;
	} catch (MelderError) {
		Melder_throw (me, ": weight not set.");
	}
}

void Network_setClamping (Network me, long nodeNumber, bool clamped) {
	try {
		if (nodeNumber <= 0 || nodeNumber > my numberOfNodes)
			Melder_throw (me, ": node number (", nodeNumber, ") out of the range 1..", my numberOfNodes, ".");
		my nodes [nodeNumber]. clamped = clamped;
	} catch (MelderError) {
		Melder_throw (me, ": clamping not set.");
	}
}

void Network_spreadActivities (Network me, long numberOfSteps) {
	for (long istep = 1; istep <= numberOfSteps; istep ++) {
		for (long inode = 1; inode <= my numberOfNodes; inode ++) {
			NetworkNode node = & my nodes [inode];
			if (! node -> clamped)
				node -> excitation -= my spreadingRate * my activityLeak * node -> excitation;
		}
		for (long iconn = 1; iconn <= my numberOfConnections; iconn ++) {
			NetworkConnection connection = & my connections [iconn];
			NetworkNode nodeFrom = & my nodes [connection -> nodeFrom];
			NetworkNode nodeTo = & my nodes [connection -> nodeTo];
			double shunting = my connections [iconn]. weight >= 0.0 ? my shunting : 0.0;   // only for excitatory connections
			if (! nodeFrom -> clamped)
				nodeFrom -> excitation += my spreadingRate * nodeTo -> activity * (my connections [iconn]. weight - shunting * nodeFrom -> excitation);
			if (! nodeTo -> clamped)
				nodeTo -> excitation += my spreadingRate * nodeFrom -> activity * (my connections [iconn]. weight - shunting * nodeTo -> excitation);
		}
		for (long inode = 1; inode <= my numberOfNodes; inode ++) {
			NetworkNode node = & my nodes [inode];
			if (! node -> clamped) {
				switch (my activityClippingRule) {
					case kNetwork_activityClippingRule_SIGMOID:
						node -> activity = my minimumActivity +
							(my maximumActivity - my minimumActivity) * NUMsigmoid (node -> excitation - 0.5 * (my minimumActivity + my maximumActivity));
					break;
					case kNetwork_activityClippingRule_LINEAR:
						if (node -> excitation < my minimumActivity) {
							node -> activity = my minimumActivity;
						} else if (node -> excitation > my maximumActivity) {
							node -> activity = my maximumActivity;
						} else {
							node -> activity = node -> excitation;
						}
					break;
					case kNetwork_activityClippingRule_TOP_SIGMOID:
						if (node -> excitation <= my minimumActivity) {
							node -> activity = my minimumActivity;
						} else {
							node -> activity = my minimumActivity +
								(my maximumActivity - my minimumActivity) * (2.0 * NUMsigmoid (2.0 * (node -> excitation - my minimumActivity) / (my maximumActivity - my minimumActivity)) - 1.0);
							trace ("excitation %f, activity %f", node -> excitation, node -> activity);
						}
					break;
				}
			}
		}
	}
}

void Network_zeroActivities (Network me, long nodeMin, long nodeMax) {
	if (my numberOfNodes < 1) return;
	if (nodeMax == 0) { nodeMin = 1; nodeMax = my numberOfNodes; }
	if (nodeMin < 1) nodeMin = 1;
	if (nodeMax > my numberOfNodes) nodeMax = my numberOfNodes;
	for (long inode = nodeMin; inode <= nodeMax; inode ++) {
		my nodes [inode]. activity = my nodes [inode]. excitation = 0.0;
	}
}

void Network_normalizeActivities (Network me, long nodeMin, long nodeMax) {
	if (my numberOfNodes < 1) return;
	if (nodeMax == 0) { nodeMin = 1; nodeMax = my numberOfNodes; }
	if (nodeMin < 1) nodeMin = 1;
	if (nodeMax > my numberOfNodes) nodeMax = my numberOfNodes;
	if (nodeMax < nodeMin) return;
	double sum = 0.0;
	for (long inode = nodeMin; inode <= nodeMax; inode ++) {
		sum += my nodes [inode]. activity;
	}
	double average = sum / (nodeMax - nodeMin + 1);
	for (long inode = nodeMin; inode <= nodeMax; inode ++) {
		my nodes [inode]. activity -= average;
	}	
}

void Network_updateWeights (Network me) {
	for (long iconn = 1; iconn <= my numberOfConnections; iconn ++) {
		NetworkConnection connection = & my connections [iconn];
		NetworkNode nodeFrom = & my nodes [connection -> nodeFrom];
		NetworkNode nodeTo = & my nodes [connection -> nodeTo];
		connection -> weight += connection -> plasticity * my learningRate *
			(nodeFrom -> activity * nodeTo -> activity - (my instar * nodeTo -> activity + my outstar * nodeFrom -> activity + my weightLeak) * connection -> weight);
		if (connection -> weight < my minimumWeight) connection -> weight = my minimumWeight;
		else if (connection -> weight > my maximumWeight) connection -> weight = my maximumWeight;
	}
}

void Network_normalizeWeights (Network me, long nodeMin, long nodeMax, long nodeFromMin, long nodeFromMax, double newSum) {
	if (my numberOfNodes < 1) return;
	if (nodeMax == 0) { nodeMin = 1; nodeMax = my numberOfNodes; }
	if (nodeMin < 1) nodeMin = 1;
	if (nodeMax > my numberOfNodes) nodeMax = my numberOfNodes;
	if (nodeMax < nodeMin) return;
	for (long inode = nodeMin; inode <= nodeMax; inode ++) {
		double sum = 0.0;
		for (long iconn = 1; iconn <= my numberOfConnections; iconn ++) {
			NetworkConnection connection = & my connections [iconn];
			if (connection -> nodeTo == inode && connection -> nodeFrom >= nodeFromMin && connection -> nodeFrom <= nodeFromMax) {
				sum += connection -> weight;
			}
		}
		if (sum != 0.0) {
			double factor = newSum / sum;
			for (long iconn = 1; iconn <= my numberOfConnections; iconn ++) {
				NetworkConnection connection = & my connections [iconn];
				if (connection -> nodeTo == inode && connection -> nodeFrom >= nodeFromMin && connection -> nodeFrom <= nodeFromMax) {
					connection -> weight *= factor;
				}
			}
		}
	}
}

Network Network_create_rectangle (double spreadingRate, enum kNetwork_activityClippingRule activityClippingRule,
	double minimumActivity, double maximumActivity, double activityLeak,
	double learningRate, double minimumWeight, double maximumWeight, double weightLeak,
	long numberOfRows, long numberOfColumns, bool bottomRowClamped,
	double initialMinimumWeight, double initialMaximumWeight)
{
	try {
		autoNetwork me = Network_create (spreadingRate, activityClippingRule, minimumActivity, maximumActivity, activityLeak,
			learningRate, minimumWeight, maximumWeight, weightLeak,
			0.0, numberOfColumns, 0.0, numberOfRows, numberOfRows * numberOfColumns,
			numberOfRows * (numberOfColumns - 1) + numberOfColumns * (numberOfRows - 1));
		/*
		 * Define nodes.
		 */
		for (long inode = 1; inode <= my numberOfNodes; inode ++) {
			NetworkNode node = & my nodes [inode];
			node -> x = (inode - 1) % numberOfColumns + 0.5;
			node -> y = (inode - 1) / numberOfColumns + 0.5;
			node -> clamped = bottomRowClamped && inode <= numberOfColumns;
			node -> activity = NUMrandomUniform (my minimumActivity, my maximumActivity);
		}
		/*
		 * Define connections.
		 */
		long iconn = 0;
		for (long irow = 1; irow <= numberOfRows; irow ++) {
			for (long icol = 1; icol <= numberOfColumns - 1; icol ++) {
				NetworkConnection conn = & my connections [++ iconn];
				conn -> nodeFrom = (irow - 1) * numberOfColumns + icol;
				conn -> nodeTo = conn -> nodeFrom + 1;
				conn -> weight = NUMrandomUniform (initialMinimumWeight, initialMaximumWeight);
				conn -> plasticity = 1.0;
			}
		}
		for (long irow = 1; irow <= numberOfRows - 1; irow ++) {
			for (long icol = 1; icol <= numberOfColumns; icol ++) {
				NetworkConnection conn = & my connections [++ iconn];
				conn -> nodeFrom = (irow - 1) * numberOfColumns + icol;
				conn -> nodeTo = conn -> nodeFrom + numberOfColumns;
				conn -> weight = NUMrandomUniform (initialMinimumWeight, initialMaximumWeight);
				conn -> plasticity = 1.0;
			}
		}
		Melder_assert (iconn == my numberOfConnections);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Rectangular network not created.");
	}
}

Network Network_create_rectangle_vertical (double spreadingRate, enum kNetwork_activityClippingRule activityClippingRule,
	double minimumActivity, double maximumActivity, double activityLeak,
	double learningRate, double minimumWeight, double maximumWeight, double weightLeak,
	long numberOfRows, long numberOfColumns, bool bottomRowClamped,
	double initialMinimumWeight, double initialMaximumWeight)
{
	try {
		autoNetwork me = Network_create (spreadingRate, activityClippingRule, minimumActivity, maximumActivity, activityLeak,
			learningRate, minimumWeight, maximumWeight, weightLeak,
			0.0, numberOfColumns, 0.0, numberOfRows, numberOfRows * numberOfColumns,
			numberOfColumns * numberOfColumns * (numberOfRows - 1));
		/*
		 * Define nodes.
		 */
		for (long inode = 1; inode <= my numberOfNodes; inode ++) {
			NetworkNode node = & my nodes [inode];
			node -> x = (inode - 1) % numberOfColumns + 0.5;
			node -> y = (inode - 1) / numberOfColumns + 0.5;
			node -> clamped = bottomRowClamped && inode <= numberOfColumns;
			node -> activity = NUMrandomUniform (my minimumActivity, my maximumActivity);
		}
		/*
		 * Define connections.
		 */
		long iconn = 0;
		for (long icol = 1; icol <= numberOfColumns; icol ++) {
			for (long jcol = 1; jcol <= numberOfColumns; jcol ++) {
				for (long irow = 1; irow <= numberOfRows - 1; irow ++) {
					NetworkConnection conn = & my connections [++ iconn];
					conn -> nodeFrom = (irow - 1) * numberOfColumns + icol;
					conn -> nodeTo = irow * numberOfColumns + jcol;
					conn -> weight = NUMrandomUniform (initialMinimumWeight, initialMaximumWeight);
					conn -> plasticity = 1.0;
				}
			}
		}
		Melder_assert (iconn == my numberOfConnections);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Vertical rectangular network not created.");
	}
}

void Network_draw (Network me, Graphics graphics, bool colour) {
	double saveLineWidth = Graphics_inqLineWidth (graphics);
	Graphics_setInner (graphics);
	Graphics_setWindow (graphics, my xmin, my xmax, my ymin, my ymax);
	if (colour) {
		Graphics_setColour (graphics, Graphics_SILVER);
		Graphics_fillRectangle (graphics, my xmin, my xmax, my ymin, my ymax);
	}
	/*
	 * Draw connections.
	 */
	for (long iconn = 1; iconn <= my numberOfConnections; iconn ++) {
		NetworkConnection conn = & my connections [iconn];
		if (fabs (conn -> weight) >= 0.01) {
			NetworkNode nodeFrom = & my nodes [conn -> nodeFrom];
			NetworkNode nodeTo = & my nodes [conn -> nodeTo];
			Graphics_setLineWidth (graphics, fabs (conn -> weight) * 6.0);
			Graphics_setColour (graphics, conn -> weight < 0.0 ? (colour ? Graphics_WHITE : Graphics_SILVER) : Graphics_BLACK);
			Graphics_line (graphics, nodeFrom -> x, nodeFrom -> y, nodeTo -> x, nodeTo -> y);
		}
	}
	Graphics_setLineWidth (graphics, 1.0);
	/*
	 * Draw the backgrounds of the nodes.
	 */
	for (long inode = 1; inode <= my numberOfNodes; inode ++) {
		NetworkNode node = & my nodes [inode];
		Graphics_setColour (graphics, colour ? Graphics_SILVER : Graphics_WHITE);
		Graphics_fillCircle_mm (graphics, node -> x, node -> y, 5.0);
	}
	/*
	 * Draw the edges of the nodes.
	 */
	Graphics_setColour (graphics, Graphics_BLACK);
	Graphics_setLineWidth (graphics, 2.0);
	for (long inode = 1; inode <= my numberOfNodes; inode ++) {
		NetworkNode node = & my nodes [inode];
		Graphics_setLineType (graphics, node -> clamped ? Graphics_DRAWN : Graphics_DOTTED);
		Graphics_circle_mm (graphics, node -> x, node -> y, 5.2);
	}
	/*
	 * Draw the activities of the nodes.
	 */
	for (long inode = 1; inode <= my numberOfNodes; inode ++) {
		NetworkNode node = & my nodes [inode];
		double activity = fabs (node -> activity);
		if (activity >= 1.0) activity = sqrt (activity);
		double diameter = activity * 5.0;
		if (diameter != 0.0) {
			Graphics_setColour (graphics,
				colour ? ( node -> activity < 0.0 ? Graphics_BLUE : Graphics_RED )
				: ( node -> activity < 0.0 ? Graphics_SILVER : Graphics_BLACK));
			Graphics_fillCircle_mm (graphics, node -> x, node -> y, diameter);
		}
	}
	Graphics_setColour (graphics, Graphics_BLACK);
	Graphics_setLineWidth (graphics, saveLineWidth);
	Graphics_setLineType (graphics, Graphics_DRAWN);
	Graphics_unsetInner (graphics);
}

void Network_addNode (Network me, double x, double y, double activity, bool clamped) {
	try {
		NUMvector_append (& my nodes, 1, & my numberOfNodes);
		my nodes [my numberOfNodes]. x = x;
		my nodes [my numberOfNodes]. y = y;
		my nodes [my numberOfNodes]. activity = my nodes [my numberOfNodes]. excitation = activity;
		my nodes [my numberOfNodes]. clamped = clamped;
	} catch (MelderError) {
		Melder_throw (me, ": node not added.");
	}
}

void Network_addConnection (Network me, long nodeFrom, long nodeTo, double weight, double plasticity) {
	try {
		NUMvector_append (& my connections, 1, & my numberOfConnections);
		my connections [my numberOfConnections]. nodeFrom = nodeFrom;
		my connections [my numberOfConnections]. nodeTo = nodeTo;
		my connections [my numberOfConnections]. weight = weight;
		my connections [my numberOfConnections]. plasticity = plasticity;
	} catch (MelderError) {
		Melder_throw (me, ": connection not added.");
	}
}

void Network_setInstar (Network me, double instar) {
	my instar = instar;
}

void Network_setOutstar (Network me, double outstar) {
	my outstar = outstar;
}

void Network_setWeightLeak (Network me, double weightLeak) {
	my weightLeak = weightLeak;
}

void Network_setActivityLeak (Network me, double activityLeak) {
	my activityLeak = activityLeak;
	Network_zeroActivities (me, 0, 0);
}

void Network_setShunting (Network me, double shunting) {
	my shunting = shunting;
	Network_zeroActivities (me, 0, 0);
}

void Network_setActivityClippingRule (Network me, enum kNetwork_activityClippingRule activityClippingRule) {
	my activityClippingRule = activityClippingRule;
	Network_zeroActivities (me, 0, 0);
}

Table Network_nodes_downto_Table (Network me, long fromNodeNumber, long toNodeNumber,
	bool includeNodeNumbers,
	bool includeX, bool includeY, int positionDecimals,
	bool includeClamped,
	bool includeActivity, bool includeExcitation, int activityDecimals)
{
	try {
		if (fromNodeNumber < 1) fromNodeNumber = 1;
		if (toNodeNumber > my numberOfNodes) toNodeNumber = my numberOfNodes;
		if (fromNodeNumber > toNodeNumber)
			fromNodeNumber = 1, toNodeNumber = my numberOfNodes;
		long numberOfNodes = toNodeNumber - fromNodeNumber + 1;
		Melder_assert (numberOfNodes >= 1);
		autoTable thee = Table_createWithoutColumnNames (numberOfNodes,
			includeNodeNumbers + includeX + includeY + includeClamped + includeActivity + includeExcitation);
		long icol = 0;
		if (includeNodeNumbers) Table_setColumnLabel (thee.peek(), ++ icol, L"node");
		if (includeX)           Table_setColumnLabel (thee.peek(), ++ icol, L"x");
		if (includeY)           Table_setColumnLabel (thee.peek(), ++ icol, L"y");
		if (includeClamped)     Table_setColumnLabel (thee.peek(), ++ icol, L"clamped");
		if (includeActivity)    Table_setColumnLabel (thee.peek(), ++ icol, L"activity");
		if (includeExcitation)  Table_setColumnLabel (thee.peek(), ++ icol, L"excitation");
		for (long inode = fromNodeNumber; inode <= toNodeNumber; inode ++) {
			NetworkNode node = & my nodes [inode];
			icol = 0;
			if (includeNodeNumbers) Table_setNumericValue (thee.peek(), inode, ++ icol, inode);
			if (includeX)           Table_setStringValue  (thee.peek(), inode, ++ icol, Melder_fixed (node -> x, positionDecimals));
			if (includeY)           Table_setStringValue  (thee.peek(), inode, ++ icol, Melder_fixed (node -> y, positionDecimals));
			if (includeClamped)     Table_setNumericValue (thee.peek(), inode, ++ icol, node -> clamped);
			if (includeActivity)    Table_setStringValue  (thee.peek(), inode, ++ icol, Melder_fixed (node -> activity,   activityDecimals));
			if (includeExcitation)  Table_setStringValue  (thee.peek(), inode, ++ icol, Melder_fixed (node -> excitation, activityDecimals));
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Table.");
	}
}

void Network_listNodes (Network me, long fromNodeNumber, long toNodeNumber,
	bool includeNodeNumbers,
	bool includeX, bool includeY, int positionDecimals,
	bool includeClamped,
	bool includeActivity, bool includeExcitation, int activityDecimals)
{
	try {
		autoTable table = Network_nodes_downto_Table (me, fromNodeNumber, toNodeNumber, includeNodeNumbers,
			includeX, includeY, positionDecimals, includeClamped, includeActivity, includeExcitation, activityDecimals);
		Table_list (table.peek(), false);
	} catch (MelderError) {
		Melder_throw (me, ": not listed.");
	}
}

/* End of file Network.cpp */
