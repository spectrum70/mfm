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

constexpr int butt_size = 20;

enum {
	id_trash,
	id_copy,
	id_cut,
	id_paste
};

struct toolbar_button : public Fl_Button
{
	toolbar_button(int X, int Y, int W, int H);

private:
	int handle(int event);
};

toolbar_button::toolbar_button(int X, int Y, int W, int H)
: Fl_Button(X, Y, W, H)
{
}

int toolbar_button::handle(int event)
{
	Fl_Button::handle(event);

	switch (event) {
	case FL_ENTER:
		color(fl_rgb_color(0xf0, 0xf0, 0xf0));
		redraw();
		break;
	case FL_LEAVE:
		color(FL_BACKGROUND_COLOR);
		redraw();
		break;
	}

	return 1;
}

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

	add_button("trash", p[id_trash]);
	add_button("copy", p[id_copy]);
	add_button("cut", p[id_cut]);
	add_button("paste", p[id_paste]);

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

	b[idx] = make_shared<toolbar_button>(0, 0, butt_size, butt_size);
	b[idx]->box(FL_FLAT_BOX);

	if (name)
		b[idx]->tooltip(name);
	if (img)
		b[idx]->image(img.get());

	idx++;

	end();
}
