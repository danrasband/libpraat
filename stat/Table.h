#ifndef _Table_h_
#define _Table_h_
/* Table.h
 *
 * Copyright (C) 2002-2011,2012 Paul Boersma
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

#include "Collection.h"
#include "Graphics.h"
#include "Interpreter_decl.h"

#ifdef PRAAT_LIB
#include "praatlib.h"
#endif

#include "Table_def.h"
oo_CLASS_CREATE (TableRow, Data);
oo_CLASS_CREATE (Table, Data);

void Table_initWithColumnNames (I, long numberOfRows, const wchar_t *columnNames);
PRAAT_LIB_EXPORT Table Table_createWithColumnNames (long numberOfRows, const wchar_t *columnNames);
void Table_initWithoutColumnNames (I, long numberOfRows, long numberOfColumns);
PRAAT_LIB_EXPORT Table Table_createWithoutColumnNames (long numberOfRows, long numberOfColumns);
#define Table_create Table_createWithoutColumnNames

Table Tables_append (Collection me);
PRAAT_LIB_EXPORT void Table_appendRow (Table me);
PRAAT_LIB_EXPORT void Table_appendColumn (Table me, const wchar_t *label);
PRAAT_LIB_EXPORT void Table_appendSumColumn (Table me, long column1, long column2, const wchar_t *label);
PRAAT_LIB_EXPORT void Table_appendDifferenceColumn (Table me, long column1, long column2, const wchar_t *label);
PRAAT_LIB_EXPORT void Table_appendProductColumn (Table me, long column1, long column2, const wchar_t *label);
PRAAT_LIB_EXPORT void Table_appendQuotientColumn (Table me, long column1, long column2, const wchar_t *label);
PRAAT_LIB_EXPORT void Table_removeRow (Table me, long row);
PRAAT_LIB_EXPORT void Table_removeColumn (Table me, long column);
PRAAT_LIB_EXPORT void Table_insertRow (Table me, long row);
PRAAT_LIB_EXPORT void Table_insertColumn (Table me, long column, const wchar_t *label);
PRAAT_LIB_EXPORT void Table_setColumnLabel (Table me, long column, const wchar_t *label);
PRAAT_LIB_EXPORT long Table_findColumnIndexFromColumnLabel (Table me, const wchar_t *label);
PRAAT_LIB_EXPORT long Table_getColumnIndexFromColumnLabel (Table me, const wchar_t *columnLabel);
PRAAT_LIB_EXPORT long * Table_getColumnIndicesFromColumnLabelString (Table me, const wchar_t *string, long *numberOfTokens);
PRAAT_LIB_EXPORT long Table_searchColumn (Table me, long column, const wchar_t *value);

/*
 * Procedure for reading strings or numbers from table cells:
 * use the following two calls exclusively.
 */
PRAAT_LIB_EXPORT const wchar_t * Table_getStringValue_Assert (Table me, long row, long column);
PRAAT_LIB_EXPORT double Table_getNumericValue_Assert (Table me, long row, long column);

/*
 * Procedure for writing strings or numbers into table cells:
 * use the following two calls exclusively.
 */
PRAAT_LIB_EXPORT void Table_setStringValue (Table me, long row, long column, const wchar_t *value);
PRAAT_LIB_EXPORT void Table_setNumericValue (Table me, long row, long column, double value);

/* For optimizations only (e.g. conversion to Matrix or TableOfReal). */
PRAAT_LIB_EXPORT void Table_numericize_Assert (Table me, long columnNumber);

PRAAT_LIB_EXPORT double Table_getQuantile (Table me, long column, double quantile);
PRAAT_LIB_EXPORT double Table_getMean (Table me, long column);
PRAAT_LIB_EXPORT double Table_getMaximum (Table me, long icol);
PRAAT_LIB_EXPORT double Table_getMinimum (Table me, long icol);
PRAAT_LIB_EXPORT double Table_getGroupMean (Table me, long column, long groupColumn, const wchar_t *group);
PRAAT_LIB_EXPORT double Table_getStdev (Table me, long column);
PRAAT_LIB_EXPORT long Table_drawRowFromDistribution (Table me, long column);
PRAAT_LIB_EXPORT double Table_getCorrelation_pearsonR (Table me, long column1, long column2, double significanceLevel,
	double *out_significance, double *out_lowerLimit, double *out_upperLimit);
PRAAT_LIB_EXPORT double Table_getCorrelation_kendallTau (Table me, long column1, long column2, double significanceLevel,
	double *out_significance, double *out_lowerLimit, double *out_upperLimit);
PRAAT_LIB_EXPORT double Table_getMean_studentT (Table me, long column, double significanceLevel,
	double *out_tFromZero, double *out_numberOfDegreesOfFreedom, double *out_significanceFromZero, double *out_lowerLimit, double *out_upperLimit);
PRAAT_LIB_EXPORT double Table_getDifference_studentT (Table me, long column1, long column2, double significanceLevel,
	double *out_t, double *out_numberOfDegreesOfFreedom, double *out_significance, double *out_lowerLimit, double *out_upperLimit);
PRAAT_LIB_EXPORT double Table_getGroupMean_studentT (Table me, long column, long groupColumn, const wchar_t *group1, double significanceLevel,
	double *out_tFromZero, double *out_numberOfDegreesOfFreedom, double *out_significanceFromZero, double *out_lowerLimit, double *out_upperLimit);
PRAAT_LIB_EXPORT double Table_getGroupDifference_studentT (Table me, long column, long groupColumn, const wchar_t *group1, const wchar_t *group2, double significanceLevel,
	double *out_tFromZero, double *out_numberOfDegreesOfFreedom, double *out_significanceFromZero, double *out_lowerLimit, double *out_upperLimit);
PRAAT_LIB_EXPORT double Table_getGroupDifference_wilcoxonRankSum (Table me, long column, long groupColumn, const wchar_t *group1, const wchar_t *group2,
	double *out_rankSum, double *out_significanceFromZero);
PRAAT_LIB_EXPORT double Table_getVarianceRatio (Table me, long column1, long column2, double significanceLevel,
	double *out_significance, double *out_lowerLimit, double *out_upperLimit);
PRAAT_LIB_EXPORT bool Table_getExtrema (Table me, long icol, double *minimum, double *maximum);

void Table_formula (Table me, long column, const wchar_t *formula, Interpreter interpreter);
void Table_formula_columnRange (Table me, long column1, long column2, const wchar_t *expression, Interpreter interpreter);

PRAAT_LIB_EXPORT void Table_sortRows_Assert (Table me, long *columns, long numberOfColumns);
PRAAT_LIB_EXPORT void Table_sortRows_string (Table me, const wchar_t *columns_string);
PRAAT_LIB_EXPORT void Table_randomizeRows (Table me);
PRAAT_LIB_EXPORT void Table_reflectRows (Table me);

void Table_scatterPlot (Table me, Graphics g, long xcolumn, long ycolumn,
	double xmin, double xmax, double ymin, double ymax, long markColumn, int fontSize, int garnish);
void Table_scatterPlot_mark (Table me, Graphics g, long xcolumn, long ycolumn,
	double xmin, double xmax, double ymin, double ymax, double markSize_mm, const wchar_t *mark, int garnish);
void Table_drawEllipse_e (Table me, Graphics g, long xcolumn, long ycolumn,
	double xmin, double xmax, double ymin, double ymax, double numberOfSigmas, int garnish);

void Table_list (Table me, bool includeRowNumbers);
PRAAT_LIB_EXPORT void Table_writeToTabSeparatedFile (Table me, MelderFile file);
PRAAT_LIB_EXPORT void Table_writeToCommaSeparatedFile (Table me, MelderFile file);
PRAAT_LIB_EXPORT Table Table_readFromTableFile (MelderFile file);
PRAAT_LIB_EXPORT Table Table_readFromCharacterSeparatedTextFile (MelderFile file, wchar_t separator);

PRAAT_LIB_EXPORT Table Table_extractRowsWhereColumn_number (Table me, long column, int which_Melder_NUMBER, double criterion);
PRAAT_LIB_EXPORT Table Table_extractRowsWhereColumn_string (Table me, long column, int which_Melder_STRING, const wchar_t *criterion);
PRAAT_LIB_EXPORT Table Table_collapseRows (Table me, const wchar_t *factors_string, const wchar_t *columnsToSum_string,
	const wchar_t *columnsToAverage_string, const wchar_t *columnsToMedianize_string,
	const wchar_t *columnsToAverageLogarithmically_string, const wchar_t *columnsToMedianizeLogarithmically_string);
PRAAT_LIB_EXPORT Table Table_rowsToColumns (Table me, const wchar_t *factors_string, long columnToTranspose, const wchar_t *columnsToExpand_string);
PRAAT_LIB_EXPORT Table Table_transpose (Table me);

PRAAT_LIB_EXPORT void Table_checkSpecifiedRowNumberWithinRange (Table me, long rowNumber);
PRAAT_LIB_EXPORT void Table_checkSpecifiedColumnNumberWithinRange (Table me, long columnNumber);
PRAAT_LIB_EXPORT bool Table_isCellNumeric_ErrorFalse (Table me, long rowNumber, long columnNumber);
PRAAT_LIB_EXPORT bool Table_isColumnNumeric_ErrorFalse (Table me, long columnNumber);

// allow C++ virtuals to be accessible via C linkage
#ifdef PRAAT_LIB
PRAAT_LIB_EXPORT double Table_getNrow (I);
PRAAT_LIB_EXPORT double Table_getNcol (I);
PRAAT_LIB_EXPORT const wchar_t * Table_getColStr (I, long columnNumber);
PRAAT_LIB_EXPORT double Table_getMatrix (I, long rowNumber, long columnNumber);
PRAAT_LIB_EXPORT const wchar_t * Table_getMatrixStr (I, long rowNumber, long columnNumber);
PRAAT_LIB_EXPORT double Table_getColIndex  (I, const wchar_t *columnLabel);
#endif

/* End of file Table.h */
#endif
