/* praat_version.cpp
 *
 * Copyright (C) 1993-2012,2013,2014 Paul Boersma
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
  * Created by Greg Hedlund for use withing jpraat bindings library
  */

#include "praat_version.h"


static struct structPraatVersion PRAAT_VERSION = {
	PRAAT_VERSION_CSTR,
	PRAAT_VERSION_NUM,
	PRAAT_YEAR,
	PRAAT_MONTH_CSTR,
	PRAAT_DAY
};

PraatVersion praat_version() {
	return &PRAAT_VERSION;
}

