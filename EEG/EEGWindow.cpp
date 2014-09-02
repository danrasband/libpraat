/* EEGWindow.cpp
 *
 * Copyright (C) 2011-2012,2013,2014 Paul Boersma
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

#include "EEGWindow.h"
#include "EditorM.h"

Thing_implement (EEGWindow, TextGridEditor, 0);

#include "prefs_define.h"
#include "EEGWindow_prefs.h"
#include "prefs_install.h"
#include "EEGWindow_prefs.h"
#include "prefs_copyToInstance.h"
#include "EEGWindow_prefs.h"

static void menu_cb_EEGWindowHelp (EDITOR_ARGS) { EDITOR_IAM (EEGWindow); Melder_help (L"EEG window"); }

void structEEGWindow :: v_createMenus () {
	EEGWindow_Parent :: v_createMenus ();
}

void structEEGWindow :: v_createHelpMenuItems (EditorMenu menu) {
	TextGridEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"EEGWindow help", '?', menu_cb_EEGWindowHelp);
}

const wchar_t * structEEGWindow :: v_getChannelName (long channelNumber) {
	Melder_assert (our d_eeg != NULL);
	return our d_eeg -> channelNames [channelNumber];
}

static void menu_cb_ExtractSelectedEEG_preserveTimes (EDITOR_ARGS) {
	EDITOR_IAM (EEGWindow);
	if (my d_endSelection <= my d_startSelection) Melder_throw ("No selection.");
	autoEEG extract = EEG_extractPart (my d_eeg, my d_startSelection, my d_endSelection, true);
	my broadcastPublication (extract.transfer());
}

static void menu_cb_ExtractSelectedEEG_timeFromZero (EDITOR_ARGS) {
	EDITOR_IAM (EEGWindow);
	if (my d_endSelection <= my d_startSelection) Melder_throw ("No selection.");
	autoEEG extract = EEG_extractPart (my d_eeg, my d_startSelection, my d_endSelection, false);
	my broadcastPublication (extract.transfer());
}

void structEEGWindow :: v_createMenuItems_file_extract (EditorMenu menu) {
	EEGWindow_Parent :: v_createMenuItems_file_extract (menu);
	our d_extractSelectedEEGPreserveTimesButton =
		EditorMenu_addCommand (menu, L"Extract selected EEG (preserve times)", 0, menu_cb_ExtractSelectedEEG_preserveTimes);
	our d_extractSelectedEEGTimeFromZeroButton =
		EditorMenu_addCommand (menu, L"Extract selected EEG (time from zero)", 0, menu_cb_ExtractSelectedEEG_timeFromZero);
}

void structEEGWindow :: v_updateMenuItems_file () {
	EEGWindow_Parent :: v_updateMenuItems_file ();
	our d_extractSelectedEEGPreserveTimesButton -> f_setSensitive (d_endSelection > d_startSelection);
	our d_extractSelectedEEGTimeFromZeroButton -> f_setSensitive (d_endSelection > d_startSelection);
}

void EEGWindow_init (EEGWindow me, const wchar_t *title, EEG eeg) {
	my d_eeg = eeg;   // before initing, because initing will already draw!
	TextGridEditor_init (me, title, eeg -> textgrid, eeg -> sound, false, NULL, NULL);
}

EEGWindow EEGWindow_create (const wchar_t *title, EEG eeg) {
	try {
		autoEEGWindow me = Thing_new (EEGWindow);
		EEGWindow_init (me.peek(), title, eeg);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("EEG window not created.");
	}
}

/* End of file EEGWindow.cpp */
