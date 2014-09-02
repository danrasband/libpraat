#ifndef _SampledXY_h_
#define _SampledXY_h_
/* SampledXY.h
 *
 * Copyright (C) 1992-2011,2013,2014 Paul Boersma
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

#include "Sampled.h"

#include "SampledXY_def.h"
oo_CLASS_CREATE (SampledXY, Sampled);

void SampledXY_init (SampledXY me, double xmin, double xmax, long nx, double dx, double x1,
                                   double ymin, double ymax, long ny, double dy, double y1);

static inline double SampledXY_indexToY (SampledXY me, long   index) { return my y1 + (index - 1  ) * my dy; }
static inline double SampledXY_indexToY (SampledXY me, double index) { return my y1 + (index - 1.0) * my dy; }
static inline double SampledXY_yToIndex (SampledXY me, double y) { return (y - my y1) / my dy + 1.0; }
static inline long SampledXY_yToLowIndex     (SampledXY me, double y) { return (long) floor ((y - my y1) / my dy + 1.0); }
static inline long SampledXY_yToHighIndex    (SampledXY me, double y) { return (long) ceil  ((y - my y1) / my dy + 1.0); }
static inline long SampledXY_yToNearestIndex (SampledXY me, double y) { return (long) round ((y - my y1) / my dy + 1.0); }

long SampledXY_getWindowSamplesY (SampledXY me, double ymin, double ymax, long *iymin, long *iymax);

/* End of file SampledXY.h */
#endif
