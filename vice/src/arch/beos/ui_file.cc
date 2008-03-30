/*
 * ui_file.cc - UI stuff dealing with files
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@arcormail.de>
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

#include <FilePanel.h>
#include <Message.h>
#include <Path.h>
#include <stdio.h>
#include <Window.h>

extern "C" {
#include "attach.h"
#include "autostart.h"
#include "machine.h"
#include "tape.h"
#include "uiapi.h"
#include "ui_file.h"
#include "utils.h"
}

static int last_fileparam[2]; /* 0=filepanel, 1=savepanel */
static int last_filetype[2];

void ui_select_file(BFilePanel *filepanel, 
					filetype_t filetype, 
					int fileparam) {

	int panelnr;
	char title[40];
	sprintf(title,"VICE filepanel"); /* default */

	/* Modify the panel */
	if (filetype == DISK_FILE)
		sprintf(title,"Attach Disk %d",fileparam);
	if (filetype == TAPE_FILE)
		sprintf(title,"Attach Tape");
	if (filetype == AUTOSTART_FILE)
		sprintf(title,"Autostart");
	if (filetype == SNAPSHOTSAVE_FILE)
		sprintf(title,"Save snapshot");
	if (filetype == SNAPSHOTLOAD_FILE)
		sprintf(title,"Load snapshot");

	filepanel->Window()->SetTitle(title);

	filepanel->Show();

	/* remember for later action */

	panelnr = (filepanel->PanelMode() == B_OPEN_PANEL ? 0 : 1);
		
	last_fileparam[panelnr] = fileparam;
	last_filetype[panelnr] = filetype;
}
	
void ui_select_file_action(BMessage *msg) {
		entry_ref 	ref;
		status_t 	err;
		BPath		*path;

	if (msg->what == B_REFS_RECEIVED) {
		/* an open action */		
		/* extract the selected filename from the message */
		if ((err = msg->FindRef("refs", 0, &ref)) != B_OK) {
			ui_error("No File selected ?!");
			return;
		}
		path = new BPath(&ref);
	
		/* now the ACTION */
    	if (last_fileparam[0] >= 8  && last_fileparam[0] <= 11) {
    		/* it's a disk-attach */
    		if (file_system_attach_disk(last_fileparam[0], path->Path()) < 0)
        		ui_error("Cannot attach specified file");
		} else if (last_fileparam[0] == 1) {
			/* it's a tape-attach */
    		if (tape_attach_image(path->Path()) < 0)
        		ui_error("Cannot attach specified file");
		} else if (last_filetype[0] == AUTOSTART_FILE) {
			if (autostart_autodetect(path->Path(), NULL, 0) < 0)
  				ui_error("Cannot autostart specified file.");
		} else if (last_filetype[0] == SNAPSHOTLOAD_FILE) {
	    	if (machine_read_snapshot(path->Path())<0) {
        		ui_error("Cannot read snapshot image");
        	}
    	}
    	
		delete path;	
	}
	
	if (msg->what == B_SAVE_REQUESTED) {
		char *fullpath;
		const char *name;
		/* a save action */
		/* first create the full path */
		if ((err = msg->FindRef("directory", &ref)) != B_OK) {
			ui_error("Wrong directory");
			return;
		}
		if ((err = msg->FindString("name", &name)) != B_OK) {
			ui_error("Wrong name");
			return;
		}
		path = new BPath(&ref);
		fullpath = concat(path->Path(),"/",name,NULL);
		
		/* now the action */
		if (last_filetype[1] == SNAPSHOTSAVE_FILE) {
			if (machine_write_snapshot(fullpath, 1, 1) < 0)
            	ui_error("Cannot write snapshot file.");
		}
		
		delete path;
		delete fullpath;
	}
}

