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

#include "table_locations.hh"
#include "table_files.hh"
#include "input.hh"
#include "config.hh"
#include "pixmaps.hh"

#include <list>
#include <FL/fl_draw.H>

using std::list;

constexpr int font_face_header = FL_HELVETICA;
constexpr int font_size_header = 11;
constexpr int font_face_row = FL_HELVETICA;
constexpr int font_size_row = 11;
constexpr int font_face_sep = FL_HELVETICA | FL_BOLD | FL_ITALIC;

static const Fl_Color color_bkgnd = fl_rgb_color(238, 238, 238);

table_locations::table_locations(int x, int y, int w, int h, app &ptrs)
: Fl_Table_Row(x, y, w, h), a(ptrs)
{
	resizable(0);

	row_resize(0);

	// Col init
	col_header(0);

	col_resize(1);
	col_width(0, 160);

	selection_color(FL_YELLOW);
	color(color_bkgnd);

	load_locations();
	load_disks();
	/* important, after first load dir */
	row_height_all(15);

	callback(__event_callback, (void*)this);

	end();
}

void table_locations::__event_callback(Fl_Widget*, void *data)
{ ((table_locations*)data)->event_callback(); }

void table_locations::event_callback()
{
	int R = callback_row();
	TableContext context = callback_context();

	switch (context) {
	case CONTEXT_CELL:
		if (Fl::event() == FL_RELEASE && Fl::event_button() == 1) {
			if (locations[R].second == ":sep")
				return;
			string loc = locations[R].second.c_str();
			if (locations[R].second.substr(0, 8) == ":disk:m:")
				loc = locations[R].second.substr(8);
			else
				loc = locations[R].second.c_str();
			a.i->value(loc.c_str());
			a.i->do_callback();
		}
		break;
	default:
		return;
	}
}

loc_path table_locations::parse_location(const string &config_location)
{
	unsigned int x;

	if ((x = config_location.find(',')) != string::npos) {
		loc_path p;

		p.first = config_location.substr(0, x);
		p.second = config_location.substr(x + 1);

		return p;
	}

	return loc_path("", "");
}

void table_locations::load_locations()
{
	unsigned int i;

	cols(1);
	locations.clear();

	locations.push_back(loc_path("Bookmarks", ":sep"));

	vect_str loc = config::get().get_string_list("bookmarks");

	for (i = 0; i < loc.size(); ++i) {
		loc_path p = parse_location(loc[i]);
		if (p.first != "") {
			locations.push_back(p);
		}
	}

	locations.push_back(loc_path("Devices", ":sep"));

	rows((int)locations.size());

	redraw();
}

void table_locations::load_disks()
{
	string script;
	char s[256];

	script = "df";
	FILE *fp = popen(script.c_str(), "r");
	vector<string> mounted;
	for (int i = 0; fgets(s, sizeof(s) - 1, fp); i++ ) {
		string entry = s;
		mounted.push_back(s);
	}
	fclose(fp);

	script = "awk -F\" \" '{print $4}' /proc/partitions";
	fp = popen(script.c_str(), "r");
	for (int i = 0; fgets(s, sizeof(s) - 1, fp); i++ ) {
		string q, entry = s;
		if (entry[entry.size() - 1] == '\n')
			entry.resize(entry.size() - 1);
		if (entry == "name" || entry == "")
			continue;
		q = ":disk";

		for (size_t j = 0; j < mounted.size(); ++j) {
			if (mounted[j].find(entry) != string::npos) {
				q += ":m:";

				size_t x = mounted[j].rfind(' ');
				if (x != string::npos) {
					string mpath =
						mounted[j].substr(x + 1);

					mpath.resize(mpath.size() - 1);
					q += mpath;
					break;
				}
			}
		}
		locations.push_back(loc_path(s, q));
	}
	fclose(fp);

	rows((int)locations.size());

	redraw();
}

void table_locations::insert(const string &location_name, const string &path)
{
	locations.push_back(loc_path(location_name, path));
	rows((int)locations.size());

	redraw();
}

void table_locations::draw_cell(TableContext context,
		int R, int C, int X, int Y, int W, int H)
{
	const char *s = "";
	const char *t = "";

	if ( R < (int) locations.size()) {
		s = locations[R].first.c_str();
		t = locations[R].second.c_str();
	}

	switch (context) {
	case CONTEXT_COL_HEADER:
		col_width(0, 160);
		fl_push_clip(X, Y, W, H); {
			fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H,
					FL_BACKGROUND_COLOR);
			fl_font(font_face_header, font_size_header);
			fl_color(FL_BLUE);
			fl_draw("locations", X + 2, Y, W, H,
						FL_ALIGN_LEFT, 0, 0);
		}
		fl_pop_clip();
		return;
	case CONTEXT_CELL: {
		fl_push_clip(X, Y, W, H); {
			if (string(t) == ":sep") {
				fl_color(color_bkgnd);
				fl_rectf(X, Y, W, H);
				fl_font(font_face_sep, font_size_row);
				fl_color(FL_BLACK);
				fl_draw(s, X + 4, Y, W, H, FL_ALIGN_LEFT |
						FL_ALIGN_BOTTOM);
			} else {
				string e = t;
				Fl_Color bgcolor = row_selected(R) ?
					selection_color() : color_bkgnd;

				fl_color(bgcolor);
				fl_rectf(X, Y, W, H);
				if (e.substr(0, 5) == ":disk") {
					if (e.substr(5, 2) == ":m") {
						fl_color(FL_YELLOW);
						fl_rectf(X + 2, Y, 16, 16);
					}
					Fl_Pixmap pm(xpm_icon_disk);
					pm.draw(X + 2, Y, 16, 16);
				} else {
					Fl_Pixmap pm(xpm_folder_blue);
					pm.draw(X + 2, Y, 16, 16);
				}

				fl_font(font_face_row, font_size_row);
				fl_color(FL_BLACK);
				fl_draw(s, X + 19, Y, W, H, FL_ALIGN_LEFT |
						FL_ALIGN_BOTTOM);
			}
		}
		fl_pop_clip();
		return;
	}
	default:
		return;
	}
}
