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

#include "toolbar.hh"
#include "input.hh"
#include "pixmaps.hh"
#include "table_files.hh"
#include "table_locations.hh"
#include "win_help.hh"
#include "win_about.hh"
#include "win_main.hh"
#include "config.hh"

#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
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
	id_paste,
	id_user_home,
	id_arrow_up,
	id_folder_new,
	id_bookmark_new,
	id_help,
	id_about,
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
	switch (event) {
	case FL_ENTER:
		color(fl_rgb_color(0xf0, 0xf0, 0xf0));
		redraw();
		return 1;
	case FL_LEAVE:
		color(FL_BACKGROUND_COLOR);
		redraw();
		return 1;
	}

	return Fl_Button::handle(event);
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
	p[id_user_home] = make_shared<Fl_Pixmap>(xpm_icon_user_home);
	p[id_arrow_up] = make_shared<Fl_Pixmap>(xpm_icon_arrow_up);
	p[id_folder_new] = make_shared<Fl_Pixmap>(xpm_icon_folder_new);
	p[id_bookmark_new] = make_shared<Fl_Pixmap>(xpm_icon_bookmark_new);
	p[id_help] = make_shared<Fl_Pixmap>(xpm_icon_help);
	p[id_about] = make_shared<Fl_Pixmap>(xpm_icon_about);

	add_button("trash", p[id_trash]);
	add_button("copy", p[id_copy]);
	add_button("cut", p[id_cut]);
	add_button("paste", p[id_paste]);
	add_separator();
	add_button("home", p[id_user_home]);
	add_button("move up", p[id_arrow_up]);
	add_separator();
	add_button("new folder", p[id_folder_new]);
	add_separator();
	add_button("add bookmark", p[id_bookmark_new]);
	add_separator();
	add_button("help", p[id_help]);
	add_button("about", p[id_about]);
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
		} else if (Fl::pushed() == b[id_copy].get()) {
			printf("handle copy\n");
			a.tf->copy();
		} else if (Fl::pushed() == b[id_cut].get()) {
			a.tf->cut();
		} else if (Fl::pushed() == b[id_paste].get()) {
			a.tf->paste();
		} else if (Fl::pushed() == b[id_user_home].get()) {
			string home = a.tf->get_home_path();
			a.i->value(home.c_str());
			a.tf->load_dir(home.c_str());
		} else if (Fl::pushed() == b[id_arrow_up].get()) {
			a.tf->update_path("..");
			a.tf->load_dir();
		} else if (Fl::pushed() == b[id_bookmark_new].get()) {
			string path = a.tf->get_cur_path();
			string name = a.tf->get_cur_folder();
			char bm[6] = {"bm"};
			int count = a.tl->rows();

			sprintf(&bm[2], "%03d", count + 1);
			config::get().add_option("bookmarks", bm,
					(name + "," + path).c_str());
			a.tl->insert(name, path);
			config::get().save_config();
		} else if (Fl::pushed() == b[id_folder_new].get()) {
			a.tf->create_folder();
		} else if (Fl::pushed() == b[id_help].get()) {
			win_help *hlp = new win_help(400, 400);
			hlp->set_modal();

			int posx = a.wm->x() + a.wm->w() / 2 - hlp->w() / 2;
			int posy = a.wm->y() + a.wm->h() / 2 - hlp->h() / 2;

			hlp->position(posx, posy);
			hlp->show();

		} else if (Fl::pushed() == b[id_about].get()) {
			win_about *abt = new win_about(450, 200);
			abt->set_modal();

			int posx = a.wm->x() + a.wm->w() / 2 - abt->w() / 2;
			int posy = a.wm->y() + a.wm->h() / 2 - abt->h() / 2;

			abt->position(posx, posy);
			abt->show();
		}

		return 1;
	case FL_RELEASE:
	case FL_DRAG:
	case FL_MOVE:
	case FL_FOCUS:
	case FL_UNFOCUS:
		return 0;
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

void toolbar::add_separator()
{
	static int idx = 0;

	seps[idx] = make_shared<Fl_Box>(w(), 0, 7, 0);

	add(seps[idx].get());
	idx++;
}
