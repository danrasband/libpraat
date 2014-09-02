#ifndef _ERPTier_h_
#define _ERPTier_h_
/* ERPTier.h
 *
 * Copyright (C) 2011,2014 Paul Boersma
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

#include "EEG.h"
#include "ERP.h"

#include "ERPTier_def.h"
oo_CLASS_CREATE (ERPPoint, AnyPoint);
oo_CLASS_CREATE (ERPTier, Function);


long ERPTier_getChannelNumber (ERPTier me, const wchar_t *channelName);
static inline void ERPTier_checkEventNumber (ERPTier me, long eventNumber) {
	if (eventNumber < 1)
		Melder_throw ("The specified event number is ", eventNumber, " but should have been positive.");
	if (eventNumber > my events -> size)
		Melder_throw ("The specified event number (", eventNumber, ") exceeds the number of events (", my events -> size, ").");
}
double ERPTier_getMean (ERPTier me, long pointNumber, long channelNumber, double tmin, double tmax);
double ERPTier_getMean (ERPTier me, long pointNumber, const wchar_t *channelName, double tmin, double tmax);
void ERPTier_subtractBaseline (ERPTier me, double tmin, double tmax);
void ERPTier_rejectArtefacts (ERPTier me, double threshold);
ERP ERPTier_extractERP (ERPTier me, long pointNumber);
ERP ERPTier_to_ERP_mean (ERPTier me);
ERPTier ERPTier_extractEventsWhereColumn_number (ERPTier me, Table table, long columnNumber, int which_Melder_NUMBER, double criterion);
ERPTier ERPTier_extractEventsWhereColumn_string (ERPTier me, Table table, long columnNumber, int which_Melder_STRING, const wchar_t *criterion);

ERPTier EEG_to_ERPTier_bit (EEG me, double fromTime, double toTime, int markerBit);
ERPTier EEG_to_ERPTier_marker (EEG me, double fromTime, double toTime, uint16_t marker);
ERPTier EEG_to_ERPTier_triggers (EEG me, double fromTime, double toTime,
	int which_Melder_STRING, const wchar_t *criterion);
ERPTier EEG_to_ERPTier_triggers_preceded (EEG me, double fromTime, double toTime,
	int which_Melder_STRING, const wchar_t *criterion,
	int which_Melder_STRING_precededBy, const wchar_t *criterion_precededBy);

/* End of file ERPTier.h */
#endif
