#ifndef _Sampled_h_
#define _Sampled_h_
/* Sampled.h
 *
 * Copyright (C) 1992-2011,2014 Paul Boersma
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

/* Sampled inherits from Function */
#include "Function.h"
#include "Graphics.h"

#ifdef PRAAT_LIB
#include "praatlib.h"
#endif

#include "Sampled_def.h"
oo_CLASS_CREATE (Sampled, Function);

/* A Sampled is a Function that is sampled at nx points [1..nx], */
/* which are spaced apart by a constant distance dx. */
/* The first sample point is at x1, the second at x1 + dx, */
/* and the last at x1 + (nx - 1) * dx. */

#ifdef PRAAT_LIB
PRAAT_LIB_EXPORT double Sampled_indexToX (Sampled me, long i);

PRAAT_LIB_EXPORT double Sampled_xToIndex (Sampled me, double x);

PRAAT_LIB_EXPORT long Sampled_xToLowIndex (Sampled me, double x);

PRAAT_LIB_EXPORT long Sampled_xToHighIndex (Sampled me, double x);

PRAAT_LIB_EXPORT long Sampled_xToNearestIndex (Sampled me, double x);
#else
static inline double Sampled_indexToX (Sampled me, long   index) { return my x1 + (index - 1  ) * my dx; }
static inline double Sampled_indexToX (Sampled me, double index) { return my x1 + (index - 1.0) * my dx; }
static inline double Sampled_xToIndex (Sampled me, double x) { return (x - my x1) / my dx + 1.0; }
static inline long Sampled_xToLowIndex     (Sampled me, double x) { return (long) floor ((x - my x1) / my dx + 1.0); }
static inline long Sampled_xToHighIndex    (Sampled me, double x) { return (long) ceil  ((x - my x1) / my dx + 1.0); }
static inline long Sampled_xToNearestIndex (Sampled me, double x) { return (long) round ((x - my x1) / my dx + 1.0); }
#endif

PRAAT_LIB_EXPORT long Sampled_getWindowSamples (Sampled me, double xmin, double xmax, long *ixmin, long *ixmax);

PRAAT_LIB_EXPORT void Sampled_init (Sampled me, double xmin, double xmax, long nx, double dx, double x1);

PRAAT_LIB_EXPORT void Sampled_shortTermAnalysis (Sampled me, double windowDuration, double timeStep,
		long *numberOfFrames, double *firstTime);
/*
	Function:
		how to put as many analysis windows of length 'windowDuration' as possible into my duration,
		when they are equally spaced by 'timeStep'.
	Input arguments:
		windowDuration:
			the duration of the analysis window, in seconds.
		timeStep:
			the time step, in seconds.
	Output arguments:
		numberOfFrames:
			at least 1 (if no failure); equals floor ((nx * dx - windowDuration) / timeStep) + 1.
		firstTime:
			the centre of the first frame, in seconds.
	Failures:
		Window longer than signal.
	Postconditions:
		the frames are divided symmetrically over my defined domain,
		which is [x1 - dx/2, x[nx] + dx/2], where x[nx] == x1 + (nx - 1) * dx.
		All analysis windows will fit into this domain.
	Usage:
		the resulting Sampled (analysis sequence, e.g., Pitch, Formant, Spectrogram, etc.)
		will have the following attributes:
			result -> xmin == my xmin;   // Copy logical domain.
			result -> xmax == my xmax;
			result -> nx == numberOfFrames;
			result -> dx == timeStep;
			result -> x1 == firstTime;
*/

PRAAT_LIB_EXPORT double Sampled_getValueAtSample (Sampled me, long isamp, long ilevel, int unit);
PRAAT_LIB_EXPORT double Sampled_getValueAtX (Sampled me, double x, long ilevel, int unit, bool interpolate);
PRAAT_LIB_EXPORT long Sampled_countDefinedSamples (Sampled me, long ilevel, int unit);
PRAAT_LIB_EXPORT double * Sampled_getSortedValues (Sampled me, long ilevel, int unit, long *numberOfValues);

PRAAT_LIB_EXPORT double Sampled_getQuantile
	(Sampled me, double xmin, double xmax, double quantile, long ilevel, int unit);
PRAAT_LIB_EXPORT double Sampled_getMean
	(Sampled me, double xmin, double xmax, long ilevel, int unit, bool interpolate);
PRAAT_LIB_EXPORT double Sampled_getMean_standardUnit
	(Sampled me, double xmin, double xmax, long ilevel, int averagingUnit, bool interpolate);
PRAAT_LIB_EXPORT double Sampled_getIntegral
	(Sampled me, double xmin, double xmax, long ilevel, int unit, bool interpolate);
PRAAT_LIB_EXPORT double Sampled_getIntegral_standardUnit
	(Sampled me, double xmin, double xmax, long ilevel, int averagingUnit, bool interpolate);
PRAAT_LIB_EXPORT double Sampled_getStandardDeviation
	(Sampled me, double xmin, double xmax, long ilevel, int unit, bool interpolate);
PRAAT_LIB_EXPORT double Sampled_getStandardDeviation_standardUnit
	(Sampled me, double xmin, double xmax, long ilevel, int averagingUnit, bool interpolate);

PRAAT_LIB_EXPORT void Sampled_getMinimumAndX (Sampled me, double xmin, double xmax, long ilevel, int unit, bool interpolate,
	double *return_minimum, double *return_xOfMinimum);
PRAAT_LIB_EXPORT double Sampled_getMinimum (Sampled me, double xmin, double xmax, long ilevel, int unit, bool interpolate);
PRAAT_LIB_EXPORT double Sampled_getXOfMinimum (Sampled me, double xmin, double xmax, long ilevel, int unit, bool interpolate);
PRAAT_LIB_EXPORT void Sampled_getMaximumAndX (Sampled me, double xmin, double xmax, long ilevel, int unit, bool interpolate,
	double *return_maximum, double *return_xOfMaximum);
PRAAT_LIB_EXPORT double Sampled_getMaximum (Sampled me, double xmin, double xmax, long ilevel, int unit, bool interpolate);
PRAAT_LIB_EXPORT double Sampled_getXOfMaximum (Sampled me, double xmin, double xmax, long ilevel, int unit, bool interpolate);

void Sampled_drawInside
	(Sampled me, Graphics g, double xmin, double xmax, double ymin, double ymax, bool speckle, long ilevel, int unit);

#ifdef PRAAT_LIB
PRAAT_LIB_EXPORT double Sampled_getXMin(Sampled me);
PRAAT_LIB_EXPORT double Sampled_getXMax(Sampled me);
PRAAT_LIB_EXPORT long Sampled_getNx(Sampled me);
PRAAT_LIB_EXPORT double Sampled_getDx(Sampled me);
PRAAT_LIB_EXPORT double Sampled_getX1(Sampled me);
#endif

/* End of file Sampled.h */
#endif
