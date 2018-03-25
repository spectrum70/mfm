/*
 * mfm
 *
 * (C) 2017 Angelo Dureghello <angelo@sysam.it>
 *
 * This file is part of mfm application.
 *
 * mfm library is free software: you can redistribute it and/or modify
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

#include "toolbar.hh"
#include "pixmaps.hh"
#include "table_files.hh"

#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Pixmap.H>

using std::make_shared;
using std::string;

constexpr int butt_size = 28;

enum {
	id_trash,
	id_copy,
	id_cut,
	id_paste
};

toolbar::toolbar(int X, int Y, int W, int H, app &ptrs)
: Fl_Pack(X, Y, W, H), a(ptrs)
{
	type(Fl_Pack::HORIZONTAL);

	box(FL_NO_BOX);
	spacing(0);

	p[id_trash] = make_shared<Fl_Pixmap>(xpm_icon_trash);
	p[id_copy] = make_shared<Fl_Pixmap>(xpm_icon_copy);
	p[id_cut] = make_shared<Fl_Pixmap>(xpm_icon_cut);
	p[id_paste] = make_shared<Fl_Pixmap>(xpm_icon_paste);

	add_button(0, p[id_trash]);
	add_button(0, p[id_copy]);
	add_button(0, p[id_cut]);
	add_button(0, p[id_paste]);

	end();
}

int toolbar::handle(int event)
{
	Fl_Group::handle(event);

	switch(event) {
	case FL_PUSH:
		if (Fl::pushed() == b[id_trash].get()) {
			string sel = a.tf->get_selected();

			if (sel == "")
				return 1;

			a.tf->trash();
		}
		break;
	case FL_RELEASE:
	case FL_DRAG:
	case FL_MOVE:
	case FL_FOCUS:
	case FL_UNFOCUS:
		return 1;
	default:
		return Fl_Pack::handle(event);
	}

	return 1;
}

void toolbar::add_button(const char *name, shared_ptr<Fl_Pixmap> &img)
{
	static int idx = 0;
	begin();

	b[idx] = make_shared<Fl_Button>(0, 0, butt_size, butt_size);
	b[idx]->box(FL_THIN_UP_BOX);
	b[idx]->clear_visible_focus();

	b[idx]->color(fl_rgb_color(120, 120, 120));
	if (name)
		b[idx]->tooltip(name);
	if (img)
		b[idx]->image(img.get());

	idx++;

	end();
}
