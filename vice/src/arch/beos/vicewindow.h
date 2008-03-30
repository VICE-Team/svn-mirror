/*
 * vicewindow.h - Implementation of the BeVICE's window
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

#ifndef _VICEWINDOW_H
#define _VICEWINDOW_H

#include <Bitmap.h>
#include <MenuBar.h>
#include <View.h>
#include <Window.h>


class ViceWindow : public BWindow {
	public:
						ViceWindow(BRect frame, char const *title);
						~ViceWindow();
		void 			Resize(unsigned int width, unsigned int height);
		void			DrawBitmap(void);
		virtual bool	QuitRequested();
		virtual void	MessageReceived(BMessage *message);

		BMenuBar		*menubar;

		BBitmap			*bitmap;
		BView			*view;
		BFilePanel		*filepanel;	
};

#endif