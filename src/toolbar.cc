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

#include <FL/Fl_Button.H>
#include <FL/Fl_Pixmap.H>

using std::make_shared;

constexpr int butt_size = 24;

void Open_CB(Fl_Widget*,void*) {
	printf("Open..\n");
}

enum {
	trash_pixmap
};

toolbar::toolbar(int X,int Y,int W,int H)  :Fl_Pack(X, Y, W, H)
{
	type(Fl_Pack::HORIZONTAL);
	box(FL_UP_FRAME);
	spacing(4);

	p[trash_pixmap] = make_shared<Fl_Pixmap>(xpm_icon_trashcan);

	add_button(0, p[trash_pixmap], Open_CB);

	end();
}

void toolbar::add_button(const char *name, shared_ptr<Fl_Pixmap> &img,
				Fl_Callback *cb, void *data)
{
	begin();

	b[0] = make_shared<Fl_Button>(0, 0, butt_size, butt_size);
	b[0]->box(FL_FLAT_BOX);
	b[0]->clear_visible_focus();
	if (name)
		b[0]->tooltip(name);
	if (img)
		b[0]->image(img.get());
	if (cb)
		b[0]->callback(cb, data);

	end();
}
