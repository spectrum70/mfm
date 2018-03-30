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

#include "win_main.hh"
#include "table_files.hh"
#include "table_locations.hh"
#include "toolbar.hh"
#include "input.hh"
#include "config.hh"

#include <globals.hh>

constexpr int window_border = 2;
constexpr int command_bar_height = 22;
constexpr int locations_width = 160;

win_main::win_main(int w, int h) : Fl_Window(w, h, "mfm")
{
	s.width = w;
	s.height = h;

	begin();

	ptrs.tb = make_shared<toolbar>(0, 2, s.width, command_bar_height, ptrs);

	ptrs.tf = make_shared<table_files>(window_border + locations_width,
			command_bar_height + 2 + 20,
			s.width - (window_border * 2) - locations_width,
			s.height - command_bar_height - 2 - window_border,
			ptrs);

	ptrs.i = make_shared<input>(2, command_bar_height + 2,
			s.width - 4, 18, ptrs);

	ptrs.tl = make_shared<table_locations>(window_border,
		command_bar_height + 2 + 20, locations_width,
		s.height - command_bar_height - 2 - window_border, ptrs);

	resizable(*ptrs.tl);
	resizable(*ptrs.tf);
	end();
	show();

	ptrs.tf->load_dir();
}

int win_main::handle(int event)
{
	if (Fl::event() == FL_KEYDOWN) {
		switch (Fl::event_key()) {
		case FL_Delete:
			/* key pressed handled here */
			ptrs.tf->trash();
			break;
		case FL_Enter: {
			string selected = ptrs.tf->get_selected();
			ptrs.tf->open_file(selected);
			focus(ptrs.tf.get());
			}
			break;
		case 'c':
			if (Fl::get_key(FL_Control_L) ||
					Fl::get_key(FL_Control_R)) {
				ptrs.tf->copy();
			}
			break;
		case 'v':
			if (Fl::get_key(FL_Control_L) ||
				Fl::get_key(FL_Control_R)) {
				ptrs.tf->paste();
			}
			break;
		case 'z':
			if (Fl::get_key(FL_Control_L) ||
				Fl::get_key(FL_Control_R)) {
				ptrs.tf->cut();
			}
			break;
		case FL_Up:
		case FL_Down:
			/*
			 * workaround, avoid arrow up/down to navigate on
			 * path window
			 * */
			return 0;
		}

	}

	return Fl_Window::handle(event);
}

int win_main::run()
{
	return Fl::run();
}
