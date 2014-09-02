#ifndef _Photo_h_
#define _Photo_h_
/* Photo.h
 *
 * Copyright (C) 2013,2014 Paul Boersma
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

#include "Photo_def.h"
oo_CLASS_CREATE (Photo, SampledXY);

void Photo_init (Photo me,
	double xmin, double xmax, long nx, double dx, double x1,
	double ymin, double ymax, long ny, double dy, double y1);

Photo Photo_create
	(double xmin, double xmax, long nx, double dx, double x1,
	 double ymin, double ymax, long ny, double dy, double y1);
/*
	Function:
		return a new opaque black Photo, or NULL if out of memory.
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
		result -> d_red -> z [1..ny] [1..nx] == 0.0;
		result -> d_green -> z [1..ny] [1..nx] == 0.0;
		result -> d_blue -> z [1..ny] [1..nx] == 0.0;
		result -> d_transparency -> z [1..ny] [1..nx] == 0.0;
*/

Photo Photo_createSimple (long numberOfRows, long numberOfColumns);
/*
	Function:
		return a new opaque black Photo, or NULL if out of memory.
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
		result -> d_red -> z [1..ny] [1..nx] == 0.0;
		result -> d_green -> z [1..ny] [1..nx] == 0.0;
		result -> d_blue -> z [1..ny] [1..nx] == 0.0;
		result -> d_transparency -> z [1..ny] [1..nx] == 0.0;
*/

Photo Photo_readFromImageFile (MelderFile file);

double_rgbt Photo_getValueAtXY (Photo me, double x, double y);
/*
	Linear interpolation between matrix points,
	constant extrapolation in cells on the edge,
	NUMundefined outside the union of the unit squares around the points.
*/

void Photo_replaceRed (Photo me, Matrix red);
void Photo_replaceGreen (Photo me, Matrix green);
void Photo_replaceBlue (Photo me, Matrix blue);
void Photo_replaceTransparency (Photo me, Matrix transparency);

void Photo_paintImage (Photo me, Graphics g, double xmin, double xmax, double ymin, double ymax);

void Photo_paintCells (Photo me, Graphics g, double xmin, double xmax, double ymin, double ymax);
/*
	Every sample is drawn as a rectangle.
*/

void Photo_movie (Photo me, Graphics g);
void Photo_saveAsPNG               (Photo me, MelderFile file);
void Photo_saveAsTIFF              (Photo me, MelderFile file);
void Photo_saveAsGIF               (Photo me, MelderFile file);
void Photo_saveAsWindowsBitmapFile (Photo me, MelderFile file);
void Photo_saveAsJPEG              (Photo me, MelderFile file);
void Photo_saveAsJPEG2000          (Photo me, MelderFile file);
void Photo_saveAsAppleIconFile     (Photo me, MelderFile file);
void Photo_saveAsWindowsIconFile   (Photo me, MelderFile file);

/* End of file Photo.h */
#endif
