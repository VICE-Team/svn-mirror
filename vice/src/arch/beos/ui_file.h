/*
 * ui_file.h - UI stuff dealing with files
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#ifndef __UI_FILE_H__
#define __UI_FILE_H__

#include <FilePanel.h>
#include <ListView.h>

const uint32 AUTOSTART_MESSAGE = 'AS01';

enum filetype_t {
	AUTOSTART_FILE,
	DISK_FILE,
	TAPE_FILE,
	SNAPSHOTSAVE_FILE,
	SNAPSHOTLOAD_FILE,
	SNAPSHOT_HISTORY_START,
	SNAPSHOT_HISTORY_END,
	C64_CARTRIDGE_FILE,
	VIC20_CARTRIDGE_FILE,
	VSID_FILE
};

class ViceFilePanel;

class VicePreview : public BWindow {
	public :
		VicePreview(BPoint origin, ViceFilePanel *f);
		void DisplayContent(char *content);
		void MessageReceived(BMessage *msg);
		BListView *contentlist;
		char image_name[256];
		ViceFilePanel *father;
};

class ViceFilePanel : public BFilePanel {
	public : 
		ViceFilePanel(file_panel_mode, BMessenger*, entry_ref*, uint32, bool);
		void SelectionChanged(void);
		void WasHidden(void);
		VicePreview *previewwindow;
};

extern void ui_select_file(ViceFilePanel *filepanel, 
					filetype_t filetype, 
					void *fileparam);

extern void ui_select_file_action(BMessage *msg);

#endif
