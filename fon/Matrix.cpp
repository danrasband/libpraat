/* Matrix.cpp
 *
 * Copyright (C) 1992-2012,2013,2014 Paul Boersma
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

#include "Matrix.h"
#include "NUM2.h"
#include "Formula.h"
#include "Eigen.h"

#include "oo_DESTROY.h"
#include "Matrix_def.h"
#include "oo_COPY.h"
#include "Matrix_def.h"
#include "oo_EQUAL.h"
#include "Matrix_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Matrix_def.h"
#include "oo_WRITE_TEXT.h"
#include "Matrix_def.h"
#include "oo_WRITE_BINARY.h"
#include "Matrix_def.h"
#include "oo_READ_BINARY.h"
#include "Matrix_def.h"
#include "oo_DESCRIPTION.h"
#include "Matrix_def.h"

Thing_implement (Matrix, SampledXY, 2);

void structMatrix :: v_info () {
	structData :: v_info ();
	double minimum = 0.0, maximum = 0.0;
	Matrix_getWindowExtrema (this, 1, our nx, 1, our ny, & minimum, & maximum);
	MelderInfo_writeLine (L"xmin: ", Melder_double (our xmin));
	MelderInfo_writeLine (L"xmax: ", Melder_double (our xmax));
	MelderInfo_writeLine (L"Number of columns: ", Melder_integer (our nx));
	MelderInfo_writeLine (L"dx: ", Melder_double (our dx), L" (-> sampling rate ", Melder_double (1.0 / our dx), L" )");
	MelderInfo_writeLine (L"x1: ", Melder_double (our x1));
	MelderInfo_writeLine (L"ymin: ", Melder_double (our ymin));
	MelderInfo_writeLine (L"ymax: ", Melder_double (our ymax));
	MelderInfo_writeLine (L"Number of rows: ", Melder_integer (our ny));
	MelderInfo_writeLine (L"dy: ", Melder_double (our dy), L" (-> sampling rate ", Melder_double (1.0 / our dy), L" )");
	MelderInfo_writeLine (L"y1: ", Melder_double (our y1));
	MelderInfo_writeLine (L"Minimum value: ", Melder_single (minimum));
	MelderInfo_writeLine (L"Maximum value: ", Melder_single (maximum));
}

void structMatrix :: v_readText (MelderReadText text) {
	if (Thing_version < 0) {
		our xmin = texgetr8 (text);
		our xmax = texgetr8 (text);
		our ymin = texgetr8 (text);
		our ymax = texgetr8 (text);
		our nx = texgeti4 (text);
		our ny = texgeti4 (text);
		our dx = texgetr8 (text);
		our dy = texgetr8 (text);
		our x1 = texgetr8 (text);
		our y1 = texgetr8 (text);
	} else {
		Matrix_Parent :: v_readText (text);
	}
	if (our xmin > our xmax)
		Melder_throw ("xmin should be less than or equal to xmax.");
	if (our ymin > our ymax)
		Melder_throw ("ymin should be less than or equal to ymax.");
	if (our nx < 1)
		Melder_throw ("nx should be at least 1.");
	if (our ny < 1)
		Melder_throw ("ny should be at least 1.");
	if (our dx <= 0.0)
		Melder_throw ("dx should be greater than 0.0.");
	if (our dy <= 0.0)
		Melder_throw ("dy should be greater than 0.0.");
	our z = NUMmatrix_readText_r8 (1, our ny, 1, our nx, text, "z");
}

double structMatrix :: v_getValueAtSample (long isamp, long ilevel, int unit) {
	double value = our z [ilevel] [isamp];
	return NUMdefined (value) ? our v_convertStandardToSpecialUnit (value, ilevel, unit) : NUMundefined;
}

double structMatrix :: v_getMatrix (long irow, long icol) {
	if (irow < 1 || irow > our ny) return 0.0;
	if (icol < 1 || icol > our nx) return 0.0;
	return z [irow] [icol];
}

double structMatrix :: v_getFunction2 (double x, double y) {
	double rrow = (y - our y1) / our dy + 1.0;
	double rcol = (x - our x1) / our dx + 1.0;
	long irow = floor (rrow), icol = floor (rcol);
	double drow = rrow - irow, dcol = rcol - icol;
	double z1 = irow < 1 || irow >  our ny || icol < 1 || icol >  our nx ? 0.0 : z [irow]     [icol];
	double z2 = irow < 0 || irow >= our ny || icol < 1 || icol >  our nx ? 0.0 : z [irow + 1] [icol];
	double z3 = irow < 1 || irow >  our ny || icol < 0 || icol >= our nx ? 0.0 : z [irow]     [icol + 1];
	double z4 = irow < 0 || irow >= our ny || icol < 0 || icol >= our nx ? 0.0 : z [irow + 1] [icol + 1];
	return (1.0 - drow) * (1.0 - dcol) * z1 + drow * (1.0 - dcol) * z2 + (1.0 - drow) * dcol * z3 + drow * dcol * z4;
}

void Matrix_init
	(Matrix me, double xmin, double xmax, long nx, double dx, double x1,
	            double ymin, double ymax, long ny, double dy, double y1)
{
	Sampled_init (me, xmin, xmax, nx, dx, x1);
	my ymin = ymin;
	my ymax = ymax;
	my ny = ny;
	my dy = dy;
	my y1 = y1;
	my z = NUMmatrix <double> (1, my ny, 1, my nx);
}

Matrix Matrix_create
	(double xmin, double xmax, long nx, double dx, double x1,
	 double ymin, double ymax, long ny, double dy, double y1)
{
	try {
		autoMatrix me = Thing_new (Matrix);
		Matrix_init (me.peek(), xmin, xmax, nx, dx, x1, ymin, ymax, ny, dy, y1);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Matrix object not created.");
	}
}

Matrix Matrix_createSimple (long numberOfRows, long numberOfColumns) {
	try {
		autoMatrix me = Thing_new (Matrix);
		Matrix_init (me.peek(), 0.5, numberOfColumns + 0.5, numberOfColumns, 1, 1,
			0.5, numberOfRows + 0.5, numberOfRows, 1, 1);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Matrix object not created.");
	}
}

double Matrix_columnToX (Matrix me, double column) { return my x1 + (column - 1) * my dx; }

double Matrix_rowToY (Matrix me, double row) { return my y1 + (row - 1) * my dy; }

double Matrix_xToColumn (Matrix me, double x) { return (x - my x1) / my dx + 1; }

long Matrix_xToLowColumn (Matrix me, double x) { return (long) floor (Matrix_xToColumn (me, x)); }

long Matrix_xToHighColumn (Matrix me, double x) { return (long) ceil (Matrix_xToColumn (me, x)); }

long Matrix_xToNearestColumn (Matrix me, double x) { return (long) floor (Matrix_xToColumn (me, x) + 0.5); }

double Matrix_yToRow (Matrix me, double y) { return (y - my y1) / my dy + 1; }

long Matrix_yToLowRow (Matrix me, double y) { return (long) floor (Matrix_yToRow (me, y)); }

long Matrix_yToHighRow (Matrix me, double y) { return (long) ceil (Matrix_yToRow (me, y)); }

long Matrix_yToNearestRow (Matrix me, double y) { return (long) floor (Matrix_yToRow (me, y) + 0.5); }

long Matrix_getWindowSamplesX (Matrix me, double xmin, double xmax, long *ixmin, long *ixmax) {
	*ixmin = 1 + (long) ceil  ((xmin - my x1) / my dx);
	*ixmax = 1 + (long) floor ((xmax - my x1) / my dx);
	if (*ixmin < 1) *ixmin = 1;
	if (*ixmax > my nx) *ixmax = my nx;
	if (*ixmin > *ixmax) return 0;
	return *ixmax - *ixmin + 1;
}

long Matrix_getWindowSamplesY (Matrix me, double ymin, double ymax, long *iymin, long *iymax) {
	*iymin = 1 + (long) ceil  ((ymin - my y1) / my dy);
	*iymax = 1 + (long) floor ((ymax - my y1) / my dy);
	if (*iymin < 1) *iymin = 1;
	if (*iymax > my ny) *iymax = my ny;
	if (*iymin > *iymax) return 0;
	return *iymax - *iymin + 1;
}

long Matrix_getWindowExtrema (Matrix me, long ixmin, long ixmax, long iymin, long iymax,
	double *minimum, double *maximum)
{
	if (ixmin == 0) ixmin = 1;
	if (ixmax == 0) ixmax = my nx;
	if (iymin == 0) iymin = 1;
	if (iymax == 0) iymax = my ny;
	if (ixmin > ixmax || iymin > iymax) return 0;
	*minimum = *maximum = my z [iymin] [ixmin];
	for (long iy = iymin; iy <= iymax; iy ++) {
		for (long ix = ixmin; ix <= ixmax; ix ++) {
			if (my z [iy] [ix] < *minimum) *minimum = my z [iy] [ix];
			if (my z [iy] [ix] > *maximum) *maximum = my z [iy] [ix];
		}
	}
	return (ixmax - ixmin + 1) * (iymax - iymin + 1);
}

double Matrix_getValueAtXY (Matrix me, double x, double y) {
	long bottomRow, leftCol, topRow, rightCol;
	double drow, dcol;
	double row_real = (y - my y1) / my dy + 1.0;
	double col_real = (x - my x1) / my dx + 1.0;
	/*
	 * We imagine a unit square around every (xi, yi) point in the matrix.
	 * For (x, y) values outside the union of these squares, the z value is undefined.
	 */
	if (row_real < 0.5 || row_real > my ny + 0.5) return NUMundefined;
	if (col_real < 0.5 || col_real > my nx + 0.5) return NUMundefined;
	/*
	 * Determine the four nearest (xi, yi) points.
	 */
	bottomRow = floor (row_real);   /* 0 <= bottomRow <= my ny */
	topRow = bottomRow + 1;         /* 1 <= topRow <= my ny + 1 */
	leftCol = floor (col_real);     /* 0 <= leftCol <= my nx */
	rightCol = leftCol + 1;         /* 1 <= rightCol <= my nx + 1 */
	drow = row_real - bottomRow;    /* 0.0 <= drow < 1.0 */
	dcol = col_real - leftCol;      /* 0.0 <= dcol < 1.0 */
	/*
	 * If adjacent points exist
	 * (i.e., both row numbers are between 1 and my ny,
	 *  or both column numbers are between 1 and my nx),
	 * we do linear interpolation.
	 * If not, we do constant extrapolation,
	 * which can be simulated by an interpolation between equal z values.
	 */
	if (bottomRow < 1) bottomRow = 1;         /* 1 <= bottomRow <= my ny */
	if (topRow > my ny) topRow = my ny;       /* 1 <= topRow <= my ny */
	if (leftCol < 1) leftCol = 1;             /* 1 <= leftCol <= my nx */
	if (rightCol > my nx) rightCol = my nx;   /* 1 <= rightCol <= my nx */
	return (1.0 - drow) * (1.0 - dcol) * my z [bottomRow] [leftCol] +
		drow * (1.0 - dcol) * my z [topRow] [leftCol] +
		(1.0 - drow) * dcol * my z [bottomRow] [rightCol] +
		drow * dcol * my z [topRow] [rightCol];
}

double Matrix_getSum (Matrix me) {
	double sum = 0.0;
	for (long row = 1; row <= my ny; row ++)
		for (long col = 1; col <= my nx; col ++)
			sum += my z [row] [col];
	return sum;
}

double Matrix_getNorm (Matrix me) {
	double sum = 0.0;
	for (long row = 1; row <= my ny; row ++)
		for (long col = 1; col <= my nx; col ++)
			sum += my z [row] [col] * my z [row] [col];
	return sqrt (sum);
}

void Matrix_drawRows (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum)
{
	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	if (ymax <= ymin) { ymin = my ymin; ymax = my ymax; }
	long ixmin, ixmax, iymin, iymax;
	(void) Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax);
	if (maximum <= minimum)
		(void) Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, iymax, & minimum, & maximum);
	if (maximum <= minimum) { minimum -= 1.0; maximum += 1.0; }
	if (xmin >= xmax) return;
	Graphics_setInner (g);
	for (long iy = iymin; iy <= iymax; iy ++) {
		Graphics_setWindow (g, xmin, xmax,
			minimum - (iy - iymin) * (maximum - minimum),
			maximum + (iymax - iy) * (maximum - minimum));
		Graphics_function (g, my z [iy], ixmin, ixmax,
			Matrix_columnToX (me, ixmin), Matrix_columnToX (me, ixmax));
	}
	Graphics_unsetInner (g);
	if (iymin < iymax)
		Graphics_setWindow (g, xmin, xmax, my y1 + (iymin - 1.5) * my dy, my y1 + (iymax - 0.5) * my dy);
}

void Matrix_drawOneContour (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double height)
{
	bool xreversed = xmin > xmax, yreversed = ymin > ymax;
	if (xmax == xmin) { xmin = my xmin; xmax = my xmax; }
	if (ymax == ymin) { ymin = my ymin; ymax = my ymax; }
	if (xreversed) { double temp = xmin; xmin = xmax; xmax = temp; }
	if (yreversed) { double temp = ymin; ymin = ymax; ymax = temp; }
	long ixmin, ixmax, iymin, iymax;
	(void) Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax);
	if (xmin == xmax || ymin == ymax) return;
	Graphics_setInner (g);
	Graphics_setWindow (g, xreversed ? xmax : xmin, xreversed ? xmin : xmax, yreversed ? ymax : ymin, yreversed ? ymin : ymax);
	Graphics_contour (g, my z,
		ixmin, ixmax, Matrix_columnToX (me, ixmin), Matrix_columnToX (me, ixmax),
		iymin, iymax, Matrix_rowToY (me, iymin), Matrix_rowToY (me, iymax),
		height);
	Graphics_rectangle (g, xmin, xmax, ymin, ymax);
	Graphics_unsetInner (g);
}

void Matrix_drawContours (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum)
{
	double border [1 + 8];
	if (xmax == xmin) { xmin = my xmin; xmax = my xmax; }
	if (ymax == ymin) { ymin = my ymin; ymax = my ymax; }
	long ixmin, ixmax, iymin, iymax, iborder;
	(void) Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax);
	if (maximum <= minimum)
		(void) Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, iymax, & minimum, & maximum);
	if (maximum <= minimum) { minimum -= 1.0; maximum += 1.0; }
	for (iborder = 1; iborder <= 8; iborder ++)
		border [iborder] = minimum + iborder * (maximum - minimum) / (8 + 1);
	if (xmin == xmax || ymin == ymax) return;
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_altitude (g, my z,
		ixmin, ixmax, Matrix_columnToX (me, ixmin), Matrix_columnToX (me, ixmax),
		iymin, iymax, Matrix_rowToY (me, iymin), Matrix_rowToY (me, iymax),
		8, border);
	Graphics_rectangle (g, xmin, xmax, ymin, ymax);
	Graphics_unsetInner (g);
}

void Matrix_paintContours (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum)
{
	double border [1 + 30];
	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	if (ymax <= ymin) { ymin = my ymin; ymax = my ymax; }
	long ixmin, ixmax, iymin, iymax, iborder;
	(void) Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax);
	if (maximum <= minimum)
		(void) Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, iymax, & minimum, & maximum);
	if (maximum <= minimum) { minimum -= 1.0; maximum += 1.0; }
	for (iborder = 1; iborder <= 30; iborder ++)
		border [iborder] = minimum + iborder * (maximum - minimum) / (30 + 1);
	if (xmin >= xmax || ymin >= ymax) return;
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_grey (g, my z,
		ixmin, ixmax, Matrix_columnToX (me, ixmin), Matrix_columnToX (me, ixmax),
		iymin, iymax, Matrix_rowToY (me, iymin), Matrix_rowToY (me, iymax),
		30, border);
	Graphics_rectangle (g, xmin, xmax, ymin, ymax);
	Graphics_unsetInner (g);
}

static void cellArrayOrImage (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum, int interpolate)
{
	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	if (ymax <= ymin) { ymin = my ymin; ymax = my ymax; }
	long ixmin, ixmax, iymin, iymax;
	(void) Matrix_getWindowSamplesX (me, xmin - 0.49999 * my dx, xmax + 0.49999 * my dx,
		& ixmin, & ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin - 0.49999 * my dy, ymax + 0.49999 * my dy,
		& iymin, & iymax);
	if (maximum <= minimum)
		(void) Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, iymax, & minimum, & maximum);
	if (maximum <= minimum) { minimum -= 1.0; maximum += 1.0; }
	if (xmin >= xmax || ymin >= ymax) return;
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	if (interpolate)
		Graphics_image (g, my z,
			ixmin, ixmax, Sampled_indexToX   (me, ixmin - 0.5), Sampled_indexToX   (me, ixmax + 0.5),
			iymin, iymax, SampledXY_indexToY (me, iymin - 0.5), SampledXY_indexToY (me, iymax + 0.5),
			minimum, maximum);
	else
		Graphics_cellArray (g, my z,
			ixmin, ixmax, Sampled_indexToX   (me, ixmin - 0.5), Sampled_indexToX   (me, ixmax + 0.5),
			iymin, iymax, SampledXY_indexToY (me, iymin - 0.5), SampledXY_indexToY (me, iymax + 0.5),
			minimum, maximum);
	Graphics_rectangle (g, xmin, xmax, ymin, ymax);
	Graphics_unsetInner (g);
}

void Matrix_paintImage (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum)
{
	cellArrayOrImage (me, g, xmin, xmax, ymin, ymax, minimum, maximum, TRUE);
}

void Matrix_paintCells (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum)
{
	cellArrayOrImage (me, g, xmin, xmax, ymin, ymax, minimum, maximum, FALSE);
}

void Matrix_paintSurface (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum, double elevation, double azimuth)
{
	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	if (ymax <= ymin) { ymin = my ymin; ymax = my ymax; }
	long ixmin, ixmax, iymin, iymax;
	(void) Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax);
	if (maximum <= minimum)
		(void) Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, iymax, & minimum, & maximum);
	if (maximum <= minimum) { minimum -= 1.0; maximum += 1.0; }
	Graphics_setInner (g);
	Graphics_setWindow (g, -1, 1, minimum, maximum);
	Graphics_surface (g, my z,
		ixmin, ixmax, Matrix_columnToX (me, ixmin), Matrix_columnToX (me, ixmax),
		iymin, iymax, Matrix_rowToY (me, iymin), Matrix_rowToY (me, iymax),
		minimum, maximum, elevation, azimuth);
	Graphics_unsetInner (g);
}

void Matrix_movie (Matrix me, Graphics g) {
	autoNUMvector <double> column (1, my ny);
	double minimum = 0.0, maximum = 1.0;
	Matrix_getWindowExtrema (me, 1, my nx, 1, my ny, & minimum, & maximum);
	Graphics_setViewport (g, 0, 1, 0, 1);
	Graphics_setWindow (g, my ymin, my ymax, minimum, maximum);
	for (long icol = 1; icol <= my nx; icol ++) {
		for (long irow = 1; irow <= my ny; irow ++)
			column [irow] = my z [irow] [icol];
		Graphics_clearWs (g);
		Graphics_function (g, column.peek(), 1, my ny, my ymin, my ymax);
		Graphics_flushWs (g);
	}
}

Matrix Matrix_readAP (MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "rb");
		short header [256];
		for (long i = 0; i < 256; i ++)
			header [i] = bingeti2LE (f);
		double samplingFrequency = header [100];
		Melder_casual ("Sampling frequency %.10g.", samplingFrequency);
		autoMatrix me = Matrix_create (0, header [34], header [34] /* Number of frames. */, 1, 0.5,
			0, header [35], header [35] /* Number of words per frame. */, 1, 0.5);
			/*Mat := MATRIX_create (Buffer.I2 [36], (* Number of words per frame. *)
							   Buffer.I2 [35], (* Number of frames. *)
							   1.0,
							   Buffer.I2 [111] / (* Samples per frame. *)
							   Buffer.I2 [101]); (* Sampling frequency. *)*/
		Melder_casual ("... Loading %d frames of %d words ...", header [34], header [35]);
		for (long i = 1; i <= my nx; i ++)
			for (long j = 1; j <= my ny; j ++)
				my z [j] [i] = bingeti2LE (f);

		/*
		 * Get pitch frequencies.
		 */
		for (long i = 1; i <= my nx; i ++)
			if (my z [1] [i] != 0.0)
				my z [1] [i] = - samplingFrequency / my z [1] [i];

		f.close (file);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Matrix object not read from AP file ", file);
	}
}

Matrix Matrix_appendRows (Matrix me, Matrix thee, ClassInfo klas) {
	try {
		autoMatrix him = (Matrix) _Thing_new (klas);
		Matrix_init (him.peek(), my xmin < thy xmin ? my xmin : thy xmin,
			my xmax > thy xmax ? my xmax : thy xmax,
			my nx > thy nx ? my nx : thy nx, my dx, my x1 < thy x1 ? my x1 : thy x1,
			my ymin, my ymax + (thy ymax - thy ymin), my ny + thy ny, my dy, my y1);
		for (long irow = 1; irow <= my ny; irow ++)
			for (long icol = 1; icol <= my nx; icol ++)
				his z [irow] [icol] = my z [irow] [icol];
		for (long irow = 1; irow <= thy ny; irow ++)
			for (long icol = 1; icol <= thy nx; icol ++)
				his z [irow + my ny] [icol] = thy z [irow] [icol];
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, " & ", thee, ": rows not appended.");
	}
}

Matrix Matrix_readFromRawTextFile (MelderFile file) {   // BUG: not Unicode-compatible
	try {
		autofile f = Melder_fopen (file, "rb");

		/*
		 * Count number of columns.
		 */
		long ncol = 0;
		for (;;) {
			int kar = fgetc (f);
			if (kar == '\n' || kar == '\r' || kar == EOF) break;
			if (kar == ' ' || kar == '\t') continue;
			ncol ++;
			do {
				kar = fgetc (f);
			} while (kar != ' ' && kar != '\t' && kar != '\n' && kar != '\r' && kar != EOF);
			if (kar == '\n' || kar == '\r' || kar == EOF) break;
		}
		if (ncol == 0)
			Melder_throw ("File empty");

		/*
		 * Count number of elements.
		 */
		rewind (f);
		long nelements = 0;
		for (;;) {
			double element;
			if (fscanf (f, "%lf", & element) < 1) break;   /* Zero or end-of-file. */
			nelements ++;
		}

		/*
		 * Check if all columns are complete.
		 */
		if (nelements == 0 || nelements % ncol != 0)
			Melder_throw ("The number of elements (", nelements, ") is not a multiple of the number of columns (", ncol, ").");

		/*
		 * Create simple matrix.
		 */
		long nrow = nelements / ncol;
		autoMatrix me = Matrix_createSimple (nrow, ncol);

		/*
		 * Read elements.
		 */
		rewind (f);
		for (long irow = 1; irow <= nrow; irow ++)
			for (long icol = 1; icol <= ncol; icol ++)
				fscanf (f, "%lf", & my z [irow] [icol]);

		f.close (file);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Matrix object not read from raw text file ", file);
	}
}

void Matrix_eigen (Matrix me, Matrix *out_eigenvectors, Matrix *out_eigenvalues) {
	*out_eigenvectors = NULL;
	*out_eigenvalues = NULL;
	try {
		if (my nx != my ny)
			Melder_throw ("(Matrix not square.");

		autoEigen eigen = Thing_new (Eigen);
		Eigen_initFromSymmetricMatrix (eigen.peek(), my z, my nx);
		autoMatrix eigenvectors = Data_copy (me);
		autoMatrix eigenvalues = Matrix_create (1, 1, 1, 1, 1, my ymin, my ymax, my ny, my dy, my y1);
		for (long i = 1; i <= my nx; i ++) {
			eigenvalues -> z [i] [1] = eigen -> eigenvalues [i];
			for (long j = 1; j <= my nx; j ++)
				eigenvectors -> z [i] [j] = eigen -> eigenvectors [j] [i];
		}
		*out_eigenvectors = eigenvectors.transfer();
		*out_eigenvalues = eigenvalues.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": eigenstructure not computed.");
	}
}

Matrix Matrix_power (Matrix me, long power) {
	try {
		if (my nx != my ny)
			Melder_throw ("Matrix not square.");
		autoMatrix thee = Data_copy (me);
		autoMatrix him = Data_copy (me);
		for (long ipow = 2; ipow <= power; ipow ++) {
			double **tmp = his z; his z = thy z; thy z = tmp;
			for (long irow = 1; irow <= my ny; irow ++) {
				for (long icol = 1; icol <= my nx; icol ++) {
					thy z [irow] [icol] = 0.0;
					for (long i = 1; i <= my nx; i ++) {
						thy z [irow] [icol] += his z [irow] [i] * my z [i] [icol];
					}
				}
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": power not computed.");
	}
}

void Matrix_writeToMatrixTextFile (Matrix me, MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "w");
		fprintf (f, "\"ooTextFile\"\n\"Matrix\"\n%.17g %.17g %ld %.17g %.17g\n%.17g %.17g %ld %.17g %.17g\n",
			my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		for (long i = 1; i <= my ny; i ++) {
			for (long j = 1; j <= my nx; j ++) {
				if (j > 1) fprintf (f, " ");
				fprintf (f, "%.17g", my z [i] [j]);
			}
			fprintf (f, "\n");
		}
		f.close (file);
	} catch (MelderError) {
		Melder_throw (me, ": not written to Matrix text file.");
	}
}

void Matrix_writeToHeaderlessSpreadsheetFile (Matrix me, MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "w");
		for (long i = 1; i <= my ny; i ++) {
			for (long j = 1; j <= my nx; j ++) {
				if (j > 1) fprintf (f, "\t");
				fprintf (f, "%ls", Melder_single (my z [i] [j]));
			}
			fprintf (f, "\n");
		}
		f.close (file);
	} catch (MelderError) {
		Melder_throw (me, ": not saved as tab-separated file ", file);
	}
}

void Matrix_formula (Matrix me, const wchar_t *expression, Interpreter interpreter, Matrix target) {
	try {
		struct Formula_Result result;
		Formula_compile (interpreter, me, expression, kFormula_EXPRESSION_TYPE_NUMERIC, TRUE);
		if (target == NULL) target = me;
		for (long irow = 1; irow <= my ny; irow ++) {
			for (long icol = 1; icol <= my nx; icol ++) {
				Formula_run (irow, icol, & result);
				target -> z [irow] [icol] = result. result.numericResult;
			}
		}
	} catch (MelderError) {
		Melder_throw (me, ": formula not completed.");
	}
}

void Matrix_formula_part (Matrix me, double xmin, double xmax, double ymin, double ymax,
	const wchar_t *expression, Interpreter interpreter, Matrix target)
{
	try {
		if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
		if (ymax <= ymin) { ymin = my ymin; ymax = my ymax; }
		long ixmin, ixmax, iymin, iymax;
		(void) Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax);
		(void) Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax);
		struct Formula_Result result;
		Formula_compile (interpreter, me, expression, kFormula_EXPRESSION_TYPE_NUMERIC, TRUE);
		if (target == NULL) target = me;
		for (long irow = iymin; irow <= iymax; irow ++) {
			for (long icol = ixmin; icol <= ixmax; icol ++) {
				Formula_run (irow, icol, & result);
				target -> z [irow] [icol] = result. result.numericResult;
			}
		}
	} catch (MelderError) {
		Melder_throw (me, ": formula not completed.");
	}
}

void Matrix_scaleAbsoluteExtremum (Matrix me, double scale) {
	double extremum = 0.0;
	for (long i = 1; i <= my ny; i ++) {
		for (long j = 1; j <= my nx; j ++) {
			if (fabs (my z [i] [j]) > extremum) {
				extremum = fabs (my z [i] [j]);
			}
		}
	}
	if (extremum != 0.0) {
		double factor = scale / extremum;
		for (long i = 1; i <= my ny; i ++) {
			for (long j = 1; j <= my nx; j ++) {
				my z [i] [j] *= factor;
			}
		}
	}
}

Matrix TableOfReal_to_Matrix (TableOfReal me) {
	try {
		autoMatrix thee = Matrix_createSimple (my numberOfRows, my numberOfColumns);
		for (long i = 1; i <= my numberOfRows; i ++)
			for (long j = 1; j <= my numberOfColumns; j ++)
				thy z [i] [j] = my data [i] [j];
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Matrix.");
	}
}

TableOfReal Matrix_to_TableOfReal (Matrix me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my ny, my nx);
		for (long i = 1; i <= my ny; i ++)
			for (long j = 1; j <= my nx; j ++)
				thy data [i] [j] = my z [i] [j];
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to TableOfReal.");
	}
}

Matrix Table_to_Matrix (Table me) {
	try {
		autoMatrix thee = Matrix_createSimple (my rows -> size, my numberOfColumns);
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			Table_numericize_Assert (me, icol);
		}
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = static_cast <TableRow> (my rows -> item [irow]);
			for (long icol = 1; icol <= my numberOfColumns; icol ++) {
				thy z [irow] [icol] = row -> cells [icol]. number;
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Matrix.");
	}
}

/* End of file Matrix.cpp */
