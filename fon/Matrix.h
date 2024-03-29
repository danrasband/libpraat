#ifndef _Matrix_h_
#define _Matrix_h_
/* Matrix.h
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

#include "SampledXY.h"
#include "Graphics.h"
#include "../stat/Table.h"
#include "../stat/TableOfReal.h"
#include "Interpreter_decl.h"

#ifdef PRAAT_LIB
#include "praatlib.h"
#endif

#include "Matrix_def.h"
oo_CLASS_CREATE (Matrix, SampledXY);

PRAAT_LIB_EXPORT void Matrix_init
	(Matrix me, double xmin, double xmax, long nx, double dx, double x1,
	            double ymin, double ymax, long ny, double dy, double y1);

PRAAT_LIB_EXPORT Matrix Matrix_create
	(double xmin, double xmax, long nx, double dx, double x1,
	 double ymin, double ymax, long ny, double dy, double y1);
/*
	Function:
		return a new empty Matrix, or NULL if out of memory.
	Preconditions:
		xmin <= xmax;   ymin <= ymax;
		nx >= 1;  ny >= 1;
		dx > 0.0;   dy > 0.0;
	Postconditions:
		result -> xmin == xmin;
		result -> xmax == xmax;
		result -> ymin == ymin;
		result -> ymax == ymax;
		result -> nx == nx;
		result -> ny == ny;
		result -> dx == dx;
		result -> dy == dy;
		result -> x1 == x1;
		result -> y1 == y1;
		result -> z [1..ny] [1..nx] == 0.0;
*/

PRAAT_LIB_EXPORT Matrix Matrix_createSimple (long numberOfRows, long numberOfColumns);
/*
	Function:
		return a new empty Matrix, or NULL if out of memory.
	Preconditions:
		numberOfRows >= 1;  numberOfColumns >= 1;
	Postconditions:
		result -> xmin == 0.5;
		result -> xmax == numberOfColumns + 0.5;
		result -> ymin == 0.5;
		result -> ymax == numberOfRows + 0.5;
		result -> nx == numberOfColumns;
		result -> ny == numberOfRows;
		result -> dx == 1;
		result -> dy == 1;
		result -> x1 == 1;
		result -> y1 == 1;
		result -> z [1..ny] [1..nx] == 0.0;
*/

/* Implemented methods

	int Matrix::writeText (I, FILE *f);
		writes a Matrix as text to the stream f.
		A sample of the format follows (see example 3 above):
			0 5000 1 2   ! xmin xmax ymin ymax
			8193 2   ! nx ny
			0.61035156 1   ! dx dy
			0 1   ! x1 y1
 			5.1e-8   ! 0 1
			5   ! 0.61035156 1
			-3.556473   ! 1.2207031 1
			...
			90000000   ! 4998.7793 2
			3.1415927   ! 4999.3896 2
 			-5.735668e35   ! 5000 2
			The data lines (all lines after the fourth) contain: my z [iy, ix], x, y.
			things written after the "!" are mere comments:
			you cannot use them to change the meaning or order of the data.

	int Matrix::readText (I, FILE *f)
		reads a Matrix as text from the stream f.
		The requested format is the same as the format produced by
		Matrix::writeText, though any comments may be left out.
		Failures:
			Wrong text input: this line should start with xmin, xmax, ymin, and ymax.
			Wrong text input: this line should start with nx >= 1 and ny >= 1.
			Wrong text input: this line should start with dx > 0.0 and dy > 0.0.
			Wrong text input: this line should start with x1 and y1.
			Out of text input: there should be 'nx * ny' lines with z values.

	int Matrix::writeBinary (I, FILE *f)
		writes a Matrix to the stream f, in IEEE floating-point format.

	int Matrix::readBinary (I, FILE *f)
		reads a Matrix from the stream f, in IEEE floating-point format.
		Failures:
			Wrong binary input.
			Not enough memory to read this Matrix.
			Out of input.
*/

PRAAT_LIB_EXPORT long Matrix_getWindowSamplesX (Matrix me, double xmin, double xmax, long *ixmin, long *ixmax);
/*
	Function:
		return the number of samples with x values in [xmin, xmax].
		Put the first of these samples in ixmin.
		Put the last of these samples in ixmax.
	Postconditions:
		*ixmin >= 1;
		*ixmax <= my nx;
		if (result != 0) *ixmin <= *ixmax; else *ixmin > *ixmax;
		if (result != 0) result == *ixmax - *ixmin + 1;
*/

PRAAT_LIB_EXPORT double Matrix_getValueAtXY (Matrix me, double x, double y);
/*
	Linear interpolation between matrix points,
	constant extrapolation in cells on the edge,
	NUMundefined outside the union of the unit squares around the points.
*/

PRAAT_LIB_EXPORT double Matrix_getSum (Matrix me);
PRAAT_LIB_EXPORT double Matrix_getNorm (Matrix me);

PRAAT_LIB_EXPORT double Matrix_columnToX (Matrix me, double column);   /* Return my x1 + (column - 1) * my dx.	 */

PRAAT_LIB_EXPORT double Matrix_rowToY (Matrix me, double row);   /* Return my y1 + (row - 1) * my dy. */

PRAAT_LIB_EXPORT double Matrix_xToColumn (Matrix me, double x);   /* Return (x - xmin) / my dx + 1. */

PRAAT_LIB_EXPORT long Matrix_xToLowColumn (Matrix me, double x);   /* Return floor (Matrix_xToColumn (me, x)). */

PRAAT_LIB_EXPORT long Matrix_xToHighColumn (Matrix me, double x);   /* Return ceil (Matrix_xToColumn (me, x)). */

PRAAT_LIB_EXPORT long Matrix_xToNearestColumn (Matrix me, double x);   /* Return floor (Matrix_xToColumn (me, x) + 0.5). */

PRAAT_LIB_EXPORT double Matrix_yToRow (Matrix me, double y);   /* Return (y - ymin) / my dy + 1. */

PRAAT_LIB_EXPORT long Matrix_yToLowRow (Matrix me, double y);   /* Return floor (Matrix_yToRow (me, y)). */

PRAAT_LIB_EXPORT long Matrix_yToHighRow (Matrix me, double x);   /* Return ceil (Matrix_yToRow (me, y)). */

PRAAT_LIB_EXPORT long Matrix_yToNearestRow (Matrix me, double y);   /* Return floor (Matrix_yToRow (me, y) + 0.5). */

PRAAT_LIB_EXPORT long Matrix_getWindowSamplesY (Matrix me, double ymin, double ymax, long *iymin, long *iymax);

PRAAT_LIB_EXPORT long Matrix_getWindowExtrema (Matrix me, long ixmin, long ixmax, long iymin, long iymax,
	double *minimum, double *maximum);
/*
	Function:
		compute the minimum and maximum values of my z over all samples inside [ixmin, ixmax] * [iymin, iymax].
	Arguments:
		if ixmin = 0, start at first column; if ixmax = 0, end at last column (same for iymin and iymax).
	Return value:
		the number of samples inside the window.
	Postconditions:
		if result == 0, *minimum and *maximum are not changed;
*/

PRAAT_LIB_EXPORT void Matrix_formula (Matrix me, const wchar_t *expression, Interpreter interpreter, Matrix target);
/*
	Arguments:
		"me" is the Matrix referred to as "self" or with "nx" etc. in the expression
		"target" is the Matrix whose elements will change according to:
			FOR row FROM 1 TO my ny
				FOR col FROM 1 TO my nx
					target -> z [row, col] = expression
				ENDFOR
			ENDFOR
		"expression" is the text to be compiled and interpreted.
		If "target" is NULL, the result will go to "me"; otherwise, to "target".
	Return value:
		0 in case of failure, otherwise 1.
*/
PRAAT_LIB_EXPORT void Matrix_formula_part (Matrix me, double xmin, double xmax, double ymin, double ymax,
	const wchar_t *expression, Interpreter interpreter, Matrix target);

/***** Graphics routines. *****/
/*
	All of these routines show the samples of a Matrix whose x and y values
	are inside the window [xmin, xmax] * [ymin, ymax].
	The scaling of the values of these samples is determined by "minimum" and "maximum".
	All of these routines can perform automatic windowing and scaling:
	if xmax <= xmin, the window in the x direction will be set to [my xmin, my xmax];
	if ymax <= ymin, the window in the y direction will be set to [my ymin, my ymax];
	if maximum <= minimum, the windowing (scaling) in the z direction will be determined
	by the minimum and maximum values of the samples inside the window.
*/

void Matrix_drawRows (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum);
/*
	Every row is plotted as a function of x,
	with straight lines connecting the sample points.
	The rows are stacked from bottom to top.
*/

void Matrix_drawOneContour (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double height);

void Matrix_drawContours (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum);
/* A contour altitude plot with curves at multiple heights. */

void Matrix_paintContours (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum);
/* A contour plot with multiple shades of grey and white (low) and black (high) paint. */

void Matrix_paintImage (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum);
/*
	Two-dimensional interpolation of greys.
	The larger the value of the sample, the darker the greys.
*/

void Matrix_paintCells (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum);
/*
	Every sample is drawn as a grey rectangle.
	The larger the value of the sample, the darker the rectangle.
*/

void Matrix_paintSurface (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum, double elevation, double azimuth);
/*
	3D surface plot. Every space between adjacent four samples is drawn as a tetragon filled with a grey value.
	'elevation' may be 30 degrees, 'azimuth' may be 45 degrees.
*/

void Matrix_movie (Matrix me, Graphics g);

PRAAT_LIB_EXPORT Matrix Matrix_readFromRawTextFile (MelderFile file);
PRAAT_LIB_EXPORT Matrix Matrix_readAP (MelderFile file);
PRAAT_LIB_EXPORT Matrix Matrix_appendRows (Matrix me, Matrix thee, ClassInfo klas);

PRAAT_LIB_EXPORT void Matrix_eigen (Matrix me, Matrix *eigenvectors, Matrix *eigenvalues);
PRAAT_LIB_EXPORT Matrix Matrix_power (Matrix me, long power);

PRAAT_LIB_EXPORT void Matrix_scaleAbsoluteExtremum (Matrix me, double scale);

PRAAT_LIB_EXPORT Matrix Table_to_Matrix (Table me);
PRAAT_LIB_EXPORT void Matrix_writeToMatrixTextFile (Matrix me, MelderFile file);
PRAAT_LIB_EXPORT void Matrix_writeToHeaderlessSpreadsheetFile (Matrix me, MelderFile file);

PRAAT_LIB_EXPORT Matrix TableOfReal_to_Matrix (TableOfReal me);
PRAAT_LIB_EXPORT TableOfReal Matrix_to_TableOfReal (Matrix me);

/* End of file Matrix.h */
#endif
