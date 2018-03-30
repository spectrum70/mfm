/*
 * mfm
 *
 * (C) 2017 Angelo Dureghello <angelo@sysam.it>
 *
 * This file is part of mfm application.
 *
 * mfm app. is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * mfm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with mfm.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "input.hh"
#include "table_files.hh"

#include <FL/Fl.H>
#include <stdio.h>

input::input(int X, int Y, int W, int H, app &ptrs)
: Fl_Input(X, Y, W, H), a(ptrs)
{
	textsize(11);
	textfont(FL_HELVETICA);

	when(FL_WHEN_ENTER_KEY_ALWAYS | FL_WHEN_CHANGED);
	callback(cb_input, (void*)this);
}

void input::cb_input(Fl_Widget *w, void *data) {
	((input *)data)->cb_event();
}

void input::cb_event()
{
	if (Fl::event() == FL_KEYDOWN) {
		if (Fl::event_key(FL_Enter)) {
			a.tf->load_dir(value());
			return;
		}
	} else if (Fl::event() == FL_RELEASE) {
		a.tf->load_dir(value());
	}
}
