#ifndef _ERP_h_
#define _ERP_h_
/* ERP.h
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

#include "Sound.h"

#include "ERP_def.h"
oo_CLASS_CREATE (ERP, Sound);

/**
	Look up the channel number from its name.
*/
long ERP_getChannelNumber (ERP me, const wchar_t *channelName);

/**
 * Draw the scalp distribution.
 * @param tmin: the time window.
 * @param tmax: the time window.
 */
void ERP_drawScalp (ERP me,
	Graphics graphics, double tmin, double tmax, double vmin, double vmax,
	enum kGraphics_colourScale colourScale, bool garnish);
void ERP_drawScalp_garnish (Graphics graphics, double vmin, double vmax, enum kGraphics_colourScale colourScale);

void ERP_drawChannel_number (ERP me, Graphics graphics, long channelNumber, double tmin, double tmax, double vmin, double vmax, bool garnish);
void ERP_drawChannel_name (ERP me, Graphics graphics, const wchar_t *channelName, double tmin, double tmax, double vmin, double vmax, bool garnish);

Table ERP_tabulate (ERP me, bool includeSampleNumbers, bool includeTime, int timeDecimals, int voltageDecimals, int units);

/**
	Extract the Sound part from the ERP. The channel names are lost.
*/
Sound ERP_downto_Sound (ERP me);

/* End of file ERP.h */
#endif
