/* ERPTier.cpp
 *
 * Copyright (C) 2011-2012,2014 Paul Boersma
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

#include "ERPTier.h"

#include "oo_DESTROY.h"
#include "ERPTier_def.h"
#include "oo_COPY.h"
#include "ERPTier_def.h"
#include "oo_EQUAL.h"
#include "ERPTier_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "ERPTier_def.h"
#include "oo_WRITE_TEXT.h"
#include "ERPTier_def.h"
#include "oo_READ_TEXT.h"
#include "ERPTier_def.h"
#include "oo_WRITE_BINARY.h"
#include "ERPTier_def.h"
#include "oo_READ_BINARY.h"
#include "ERPTier_def.h"
#include "oo_DESCRIPTION.h"
#include "ERPTier_def.h"

/***** ERPPoint *****/

Thing_implement (ERPPoint, AnyPoint, 0);

/***** ERPTier *****/

Thing_implement (ERPTier, Function, 0);

void structERPTier :: v_shiftX (double xfrom, double xto) {
	ERPTier_Parent :: v_shiftX (xfrom, xto);
	//if (our sound    != NULL)  Function_shiftXTo (our sound,    xfrom, xto);
	//if (our textgrid != NULL)  Function_shiftXTo (our textgrid, xfrom, xto);
}

void structERPTier :: v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	ERPTier_Parent :: v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	//if (our sound    != NULL)  our sound    -> v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	//if (our textgrid != NULL)  our textgrid -> v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
}

long ERPTier_getChannelNumber (ERPTier me, const wchar_t *channelName) {
	for (long ichan = 1; ichan <= my numberOfChannels; ichan ++) {
		if (Melder_wcsequ (my channelNames [ichan], channelName)) {
			return ichan;
		}
	}
	return 0;
}

double ERPTier_getMean (ERPTier me, long pointNumber, long channelNumber, double tmin, double tmax) {
	if (pointNumber < 1 || pointNumber > my events -> size) return NUMundefined;
	if (channelNumber < 1 || channelNumber > my numberOfChannels) return NUMundefined;
	ERPPoint point = my event (pointNumber);
	return Vector_getMean (point -> erp, tmin, tmax, channelNumber);
}

double ERPTier_getMean (ERPTier me, long pointNumber, const wchar_t *channelName, double tmin, double tmax) {
	return ERPTier_getMean (me, pointNumber, ERPTier_getChannelNumber (me, channelName), tmin, tmax);
}

static ERPTier EEG_PointProcess_to_ERPTier (EEG me, PointProcess events, double fromTime, double toTime) {
	try {
		autoERPTier thee = Thing_new (ERPTier);
		Function_init (thee.peek(), fromTime, toTime);
		thy numberOfChannels = my numberOfChannels - EEG_getNumberOfExtraSensors (me);
		Melder_assert (thy numberOfChannels > 0);
		thy channelNames = NUMvector <wchar_t *> (1, thy numberOfChannels);
		for (long ichan = 1; ichan <= thy numberOfChannels; ichan ++) {
			thy channelNames [ichan] = Melder_wcsdup (my channelNames [ichan]);
		}
		long numberOfEvents = events -> nt;
		thy events = SortedSetOfDouble_create ();
		double soundDuration = toTime - fromTime;
		double samplingPeriod = my sound -> dx;
		long numberOfSamples = floor (soundDuration / samplingPeriod) + 1;
		if (numberOfSamples < 1)
			Melder_throw (L"Time window too short.");
		double midTime = 0.5 * (fromTime + toTime);
		double soundPhysicalDuration = numberOfSamples * samplingPeriod;
		double firstTime = midTime - 0.5 * soundPhysicalDuration + 0.5 * samplingPeriod;   // distribute the samples evenly over the time domain
		for (long ievent = 1; ievent <= numberOfEvents; ievent ++) {
			double eegEventTime = events -> t [ievent];
			autoERPPoint event = Thing_new (ERPPoint);
			event -> number = eegEventTime;
			event -> erp = Sound_create (thy numberOfChannels, fromTime, toTime, numberOfSamples, samplingPeriod, firstTime);
			double erpEventTime = 0.0;
			double eegSample = 1 + (eegEventTime - my sound -> x1) / samplingPeriod;
			double erpSample = 1 + (erpEventTime - firstTime) / samplingPeriod;
			long sampleDifference = round (eegSample - erpSample);
			for (long ichannel = 1; ichannel <= thy numberOfChannels; ichannel ++) {
				for (long isample = 1; isample <= numberOfSamples; isample ++) {
					long jsample = isample + sampleDifference;
					event -> erp -> z [ichannel] [isample] = jsample < 1 || jsample > my sound -> nx ? 0.0 : my sound -> z [ichannel] [jsample];
				}
			}
			Collection_addItem (thy events, event.transfer());
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": ERP analysis not performed.");
	}
}

ERPTier EEG_to_ERPTier_bit (EEG me, double fromTime, double toTime, int markerBit) {
	try {
		autoPointProcess events = TextGrid_getStartingPoints (my textgrid, markerBit, kMelder_string_EQUAL_TO, L"1");
		autoERPTier thee = EEG_PointProcess_to_ERPTier (me, events.peek(), fromTime, toTime);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": ERPTier not created.");
	}
}

static PointProcess TextGrid_getStartingPoints_multiNumeric (TextGrid me, uint16_t number) {
	try {
		autoPointProcess thee = NULL;
		int numberOfBits = my numberOfTiers();
		for (int ibit = 0; ibit < numberOfBits; ibit ++) {
			(void) TextGrid_checkSpecifiedTierIsIntervalTier (me, ibit + 1);
			if (number & (1 << ibit)) {
				autoPointProcess bitEvents = TextGrid_getStartingPoints (me, ibit + 1, kMelder_string_EQUAL_TO, L"1");
				if (thee.peek()) {
					autoPointProcess intersection = PointProcesses_intersection (thee.peek(), bitEvents.peek());
					thee.reset (intersection.transfer());
				} else {
					thee.reset (bitEvents.transfer());
				}
			}
		}
		for (int ibit = 0; ibit < numberOfBits; ibit ++) {
			autoPointProcess bitEvents = TextGrid_getStartingPoints (me, ibit + 1, kMelder_string_EQUAL_TO, L"1");
			if (! (number & (1 << ibit))) {
				if (thee.peek()) {
					autoPointProcess difference = PointProcesses_difference (thee.peek(), bitEvents.peek());
					thee.reset (difference.transfer());
				} else {
					thee.reset (PointProcess_create (my xmin, my xmax, 10));
				}
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": starting points not converted to PointProcess.");
	}
}

ERPTier EEG_to_ERPTier_marker (EEG me, double fromTime, double toTime, uint16_t marker) {
	try {
		autoPointProcess events = TextGrid_getStartingPoints_multiNumeric (my textgrid, marker);
		autoERPTier thee = EEG_PointProcess_to_ERPTier (me, events.peek(), fromTime, toTime);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": ERPTier not created.");
	}
}

ERPTier EEG_to_ERPTier_triggers (EEG me, double fromTime, double toTime,
	int which_Melder_STRING, const wchar_t *criterion)
{
	try {
		autoPointProcess events = TextGrid_getPoints (my textgrid, 2, which_Melder_STRING, criterion);
		autoERPTier thee = EEG_PointProcess_to_ERPTier (me, events.peek(), fromTime, toTime);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": ERPTier not created.");
	}
}

ERPTier EEG_to_ERPTier_triggers_preceded (EEG me, double fromTime, double toTime,
	int which_Melder_STRING, const wchar_t *criterion,
	int which_Melder_STRING_precededBy, const wchar_t *criterion_precededBy)
{
	try {
		autoPointProcess events = TextGrid_getPoints_preceded (my textgrid, 2,
			which_Melder_STRING, criterion,
			which_Melder_STRING_precededBy, criterion_precededBy);
		autoERPTier thee = EEG_PointProcess_to_ERPTier (me, events.peek(), fromTime, toTime);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": ERPTier not created.");
	}
}

void ERPTier_subtractBaseline (ERPTier me, double tmin, double tmax) {
	long numberOfEvents = my events -> size;
	if (numberOfEvents < 1)
		return;   // nothing to do
	ERPPoint firstEvent = my event (1);
	long numberOfChannels = firstEvent -> erp -> ny;
	long numberOfSamples = firstEvent -> erp -> nx;
	for (long ievent = 1; ievent <= numberOfEvents; ievent ++) {
		ERPPoint event = my event (ievent);
		for (long ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
			double mean = Vector_getMean (event -> erp, tmin, tmax, ichannel);
			double *channel = event -> erp -> z [ichannel];
			for (long isample = 1; isample <= numberOfSamples; isample ++) {
				channel [isample] -= mean;
			}
		}
	}
}

void ERPTier_rejectArtefacts (ERPTier me, double threshold) {
	long numberOfEvents = my events -> size;
	if (numberOfEvents < 1)
		return;   // nothing to do
	ERPPoint firstEvent = my event (1);
	long numberOfChannels = firstEvent -> erp -> ny;
	long numberOfSamples = firstEvent -> erp -> nx;
	if (numberOfSamples < 1)
		return;   // nothing to do
	for (long ievent = numberOfEvents; ievent >= 1; ievent --) {   // cycle down because of removal
		ERPPoint event = my event (ievent);
		double minimum = event -> erp -> z [1] [1];
		double maximum = minimum;
		for (long ichannel = 1; ichannel <= (numberOfChannels & ~ 15); ichannel ++) {
			double *channel = event -> erp -> z [ichannel];
			for (long isample = 1; isample <= numberOfSamples; isample ++) {
				double value = channel [isample];
				if (value < minimum) minimum = value;
				if (value > maximum) maximum = value;
			}
		}
		if (minimum < - threshold || maximum > threshold) {
			Collection_removeItem (my events, ievent);
		}
	}
}

ERP ERPTier_extractERP (ERPTier me, long eventNumber) {
	try {
		long numberOfEvents = my events -> size;
		if (numberOfEvents < 1)
			Melder_throw ("No events.");
		ERPTier_checkEventNumber (me, eventNumber);
		ERPPoint event = my event (eventNumber);
		long numberOfChannels = event -> erp -> ny;
		long numberOfSamples = event -> erp -> nx;
		autoERP thee = Thing_new (ERP);
		event -> erp -> structSound :: v_copy (thee.peek());
		for (long ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
			double *oldChannel = event -> erp -> z [ichannel];
			double *newChannel = thy z [ichannel];
			for (long isample = 1; isample <= numberOfSamples; isample ++) {
				newChannel [isample] = oldChannel [isample];
			}
		}
		thy channelNames = NUMvector <wchar_t *> (1, thy ny);
		for (long ichan = 1; ichan <= thy ny; ichan ++) {
			thy channelNames [ichan] = Melder_wcsdup (my channelNames [ichan]);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": ERP not extracted.");
	}
}

ERP ERPTier_to_ERP_mean (ERPTier me) {
	try {
		long numberOfEvents = my events -> size;
		if (numberOfEvents < 1)
			Melder_throw ("No events.");
		ERPPoint firstEvent = my event (1);
		long numberOfChannels = firstEvent -> erp -> ny;
		long numberOfSamples = firstEvent -> erp -> nx;
		autoERP mean = Thing_new (ERP);
		firstEvent -> erp -> structSound :: v_copy (mean.peek());
		for (long ievent = 2; ievent <= numberOfEvents; ievent ++) {
			ERPPoint event = my event (ievent);
			for (long ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
				double *erpChannel = event -> erp -> z [ichannel];
				double *meanChannel = mean -> z [ichannel];
				for (long isample = 1; isample <= numberOfSamples; isample ++) {
					meanChannel [isample] += erpChannel [isample];
				}
			}
		}
		double factor = 1.0 / numberOfEvents;
		for (long ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
			double *meanChannel = mean -> z [ichannel];
			for (long isample = 1; isample <= numberOfSamples; isample ++) {
				meanChannel [isample] *= factor;
			}
		}
		mean -> channelNames = NUMvector <wchar_t *> (1, mean -> ny);
		for (long ichan = 1; ichan <= mean -> ny; ichan ++) {
			mean -> channelNames [ichan] = Melder_wcsdup (my channelNames [ichan]);
		}
		return mean.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": mean not computed.");
	}
}

ERPTier ERPTier_extractEventsWhereColumn_number (ERPTier me, Table table, long columnNumber, int which_Melder_NUMBER, double criterion) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (table, columnNumber);
		Table_numericize_Assert (table, columnNumber);   // extraction should work even if cells are not defined
		if (my events -> size != table -> rows -> size)
			Melder_throw (me, " & ", table, ": the number of rows in the table (", table -> rows -> size,
				") doesn't match the number of events (", my events -> size, ").");
		autoERPTier thee = Thing_new (ERPTier);
		Function_init (thee.peek(), my xmin, my xmax);
		thy numberOfChannels = my numberOfChannels;
		thy channelNames = NUMvector <wchar_t *> (1, thy numberOfChannels);
		for (long ichan = 1; ichan <= thy numberOfChannels; ichan ++) {
			thy channelNames [ichan] = Melder_wcsdup (my channelNames [ichan]);
		}
		thy events = SortedSetOfDouble_create ();
		for (long ievent = 1; ievent <= my events -> size; ievent ++) {
			ERPPoint oldEvent = my event (ievent);
			TableRow row = table -> row (ievent);
			if (Melder_numberMatchesCriterion (row -> cells [columnNumber]. number, which_Melder_NUMBER, criterion)) {
				autoERPPoint newEvent = Data_copy (oldEvent);
				Collection_addItem (thy events, newEvent.transfer());
			}
		}
		if (thy events -> size == 0) {
			Melder_warning ("No event matches criterion.");
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": events not extracted.");
	}
}

ERPTier ERPTier_extractEventsWhereColumn_string (ERPTier me, Table table,
	long columnNumber, int which_Melder_STRING, const wchar_t *criterion)
{
	try {
		Table_checkSpecifiedColumnNumberWithinRange (table, columnNumber);
		if (my events -> size != table -> rows -> size)
			Melder_throw (me, " & ", table, ": the number of rows in the table (", table -> rows -> size,
				") doesn't match the number of events (", my events -> size, ").");
		autoERPTier thee = Thing_new (ERPTier);
		Function_init (thee.peek(), my xmin, my xmax);
		thy numberOfChannels = my numberOfChannels;
		thy channelNames = NUMvector <wchar_t *> (1, thy numberOfChannels);
		for (long ichan = 1; ichan <= thy numberOfChannels; ichan ++) {
			thy channelNames [ichan] = Melder_wcsdup (my channelNames [ichan]);
		}
		thy events = SortedSetOfDouble_create ();
		for (long ievent = 1; ievent <= my events -> size; ievent ++) {
			ERPPoint oldEvent = my event (ievent);
			TableRow row = table -> row (ievent);
			if (Melder_stringMatchesCriterion (row -> cells [columnNumber]. string, which_Melder_STRING, criterion)) {
				autoERPPoint newEvent = Data_copy (oldEvent);
				Collection_addItem (thy events, newEvent.transfer());
			}
		}
		if (thy events -> size == 0) {
			Melder_warning ("No event matches criterion.");
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": events not extracted.");
	}
}

/* End of file ERPTier.cpp */
