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

#include "table_files.hh"
#include "input.hh"

#include <algorithm>
#include <sys/types.h>
#include <grp.h>
#include <unistd.h>

#include <FL/fl_draw.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Menu_.H>
#include <FL/Fl_Menu_Item.H>

constexpr int font_face_header = FL_HELVETICA;
constexpr int font_size_header = 11;
constexpr int font_face_row = FL_HELVETICA;
constexpr int font_size_row = 11;
constexpr int max_username_len = 32;

constexpr int COLUMNS = 9;
constexpr int LS_OUT_COLS = 11;

static const char *header[] = {
		"cnt",
		"name",
		"size",
		"date",
		"time",
		"rights",
		"owner",
		"group",
		"link path",
		0
};

enum {
	cl_op_none,
	cl_op_copy,
	cl_op_cut,
};

// Sort class to handle sorting column using std::sort
class sort_method {
	int _col, _reverse;
public:
	sort_method(int col, int reverse) {
		_col = col;
		_reverse = reverse;
	}
	bool operator()(const row &a, const row &b) {
		const char *ap = ( _col < (int)a.cols.size() ) ?
				a.cols[_col] : "",
			   *bp = ( _col < (int)b.cols.size() ) ?
				b.cols[_col] : "";
		if ( isdigit(*ap) && isdigit(*bp) ) {
			// Numeric sort
			int av=0; sscanf(ap, "%d", &av);
			int bv=0; sscanf(bp, "%d", &bv);

			return( _reverse ? av < bv : bv < av );
		} else {
			// Alphabetic sort
			return( _reverse ? strcmp(ap, bp) > 0 :
					strcmp(ap, bp) < 0 );
		}
	}
};

table_files::table_files(int x, int y, int w, int h, app &ptrs)
: path(), Fl_Table_Row(x, y, w, h), a(ptrs), sort_reverse(0), sort_lastcol(0)
{
	current_usr.resize(max_username_len);
	getlogin_r(&current_usr[0], max_username_len);
	current_usr.resize(strlen(&current_usr[0]));

	struct group * g = getgrgid(getuid());
	current_grp = g->gr_name;

	row_resize(0);

	col_header(1);
	col_header_height(14);
	col_resize(1);

	selection_color(FL_YELLOW);
	color(FL_WHITE);

	callback(__event_callback, (void*)this);

	focus(this);

	end();
}

void table_files::autowidth(int pad)
{
	int w, h;

	// Initialize all column widths to header width
	fl_font(font_face_header, font_size_header);

	for ( int c = 0; header[c]; c++ ) {
		w = 0;
		fl_measure(header[c], w, h, 0);
		col_width(c, w + pad);
	}

	fl_font(font_face_row, font_size_row);

	for ( int r=0; r<(int)rowdata.size(); r++ ) {
		for ( int c = 0; c < (int)rowdata[r].cols.size(); c++ ) {
			w = 0;
			fl_measure(rowdata[r].cols[c], w, h, 0);
			if ( (w + pad) > col_width(c))
				col_width(c, w + pad);
		}
	}
	table_resized();
	redraw();
}

void table_files::open_file(string &file)
{
	static char s[512];
	string file_path;

	if (file.find(' ') != string::npos) {
		file.insert(0, "'");
		file.push_back('\'');
	}

	file_path = (string(fs_path) + "/" + file).c_str();

	snprintf(s, sizeof s, "%s %s &", "xdg-open", file_path.c_str());

	system(s);
}

void table_files::handle_rmenu(Fl_Widget *w)
{
	Fl_Menu_Item *mi = (Fl_Menu_Item *)w;

	if (strncmp(mi->label(), "cut", 3) == 0) {
		cut();
	} else if (strncmp(mi->label(), "copy", 4) == 0) {
		copy();
	} else if (strncmp(mi->label(), "paste", 5) == 0) {
		paste();
	} else if (strncmp(mi->label(), "delete", 6) == 0) {
		trash();
	} else if (strncmp(mi->label(), "rename", 6) == 0) {
	}
}

void table_files::event_callback()
{
	int R = callback_row();
	int C = callback_col();
	TableContext context = callback_context();

	switch ( context ) {
	case CONTEXT_COL_HEADER: {
		// someone clicked on column header
		if (Fl::event() == FL_RELEASE && Fl::event_button() == 1 ) {
			if (sort_lastcol == C) {
				sort_reverse ^= 1;
			} else {
				sort_reverse = 0;
			}
			sort_column(C, sort_reverse);
			sort_lastcol = C;
		}
		break;
	case CONTEXT_CELL:
		if (Fl::event() == FL_RELEASE && Fl::event_button() == 3) {
			select_row(R, 1);
			selected = rowdata[R].cols[1];

			Fl_Menu_Item rclick_menu[] = {
				{"cut", FL_CTRL + 'z', __handle_rmenu, this},
				{"copy", FL_CTRL + 'c', __handle_rmenu, this},
				{"paste", FL_CTRL + 'v', __handle_rmenu, this,
					FL_MENU_DIVIDER},
				{"delete", FL_Delete, __handle_rmenu, this,
					FL_MENU_DIVIDER},
				{"rename", 0, __handle_rmenu, this},
				{0}
			};

			const Fl_Menu_Item *m =
				rclick_menu->popup(
					Fl::event_x(), Fl::event_y(), 0, 0, 0);
    			if (m) {
				m->do_callback((Fl_Widget *)m, m->user_data());
				load_dir();
			}
		} else if (Fl::event() == FL_KEYDOWN) {
			if (Fl::event_key() == FL_Up)
				move_selection_up();
			else if (Fl::event_key() == FL_Down)
				move_selection_down();

			selected = rowdata[R].cols[1];

			return;
		}
		if (Fl::event() != FL_RELEASE || Fl::event_button() != 1)
			return;
		if (C != 1)
			return;
		if (rowdata[R].cols[5][0] == 'd') {
			update_path(rowdata[R].cols[1]);
			load_dir();
		} else if (rowdata[R].cols[5][0] == 'l') {
			update_path(rowdata[R].cols[COLUMNS]);
			load_dir();
		} else {
			if (rowdata[R].cols[1] != selected) {
				selected = rowdata[R].cols[1];
				return;
			}
			open_file(selected);
		}
		/* invalidating */
		selected = "";
		break;
	}
	default:
	return;
	}
}

void table_files::move_selection_up()
{
	int row_top, col_left, row_bot, col_right;

	get_selection(row_top, col_left, row_bot, col_right);

	if (row_top < 0)
		return;

	select_row(row_top + 1, 0);
	select_row(row_top, 1);

	set_selection(row_top, col_left, row_bot, col_right);
}

void table_files::move_selection_down()
{
	int row_top, col_left, row_bot, col_right;

	get_selection(row_top, col_left, row_bot, col_right);

	select_row(row_top - 1, 0);
	select_row(row_top, 1);
	set_selection(row_top, col_left, row_bot, col_right);
}

void table_files::trash()
{
	string text = "delete [ " + selected + " ] ?";

	if (fl_choice(text.c_str(), "Yes", "No", 0) == 0) {
		unlink((string(fs_path) + "/" + selected).c_str());
	}

	load_dir();
}

void table_files::copy()
{
	clip_op_src = string(fs_path) + "/" + selected;

	cl_op = cl_op_copy;

	focus(this);
}

void table_files::cut()
{
	clip_op_src = string(fs_path) + "/" + selected;

	cl_op = cl_op_cut;

	focus(this);
}

void table_files::paste()
{
	string cmd;
	string dest;
	int x;

	if ((x = clip_op_src.rfind('/')) != string::npos) {
		dest = clip_op_src.substr(x + 1);
	}

	if (cl_op = cl_op_copy) {
		cmd = "cp ";
	} else if (cl_op = cl_op_cut) {
		cmd = "mv ";
	}

	cmd += clip_op_src + " " + string(fs_path) + "/" + dest;

	system(cmd.c_str());

	load_dir();

	focus(this);
}

void table_files::sort_column(int col, int reverse)
{
	std::sort(rowdata.begin(), rowdata.end(), sort_method(col, reverse));
	redraw();
}

void table_files::load_dir(const char *path)
{
	char s[512];
	char n[16];
	const int remap[LS_OUT_COLS] = {5, 0, 6, 7, 2, 3, 0, 4, 1, 0, 0};

	rowdata.clear();

	if (path)
		fs_path = path;

	a.i->value(fs_path.c_str());

	strcpy(s, "ls -alh ");
	strcat(s, ls_normalize(fs_path).c_str());

	FILE *fp = popen(s, "r");
	cols(0);
	for (int i = 0; fgets(s, sizeof(s) - 1, fp); i++ ) {
		// Add a new row
		row r;
		char *ss;
		const char *delim = " \t\n";

		if (i == 0)
			continue;

		rowdata.push_back(r);

		vector<char*> &rc = rowdata[i - 1].cols;
		rc.resize(COLUMNS + 1);

		// Break the line
		for (int t = 0; t < LS_OUT_COLS; t++) {
			if (t == 0) {
				ss = strtok(s, delim);
				sprintf(n, "%d", i);
				rc[0] = strdup(n);
			} else if (t == 8) {
				bool link = false;
				longname[0] = 0;
				while (ss = strtok(NULL, delim)) {
					if (link) {
						/* store link path */
						rc[COLUMNS - 1] = strdup(ss);
						break;
					}
					if (string(ss) == "->") {
						link = true;
						continue;
					} else {
						if (longname[0])
							strcat(longname, " ");
						strcat(longname, ss);
					}
				}
				ss = longname;
			} else {
				ss = strtok(NULL, delim);
			}

			if (t == 5) {
				strcpy(n, ss);
			} else if (t == 6) {
				strcat(n, " ");
				strcat(n, ss);
				rc[remap[5]] = strdup(n);
			}

			if (remap[t])
				rc[remap[t]] = (strdup(ss));
		}
		// Keep track of max # columns
		if ((int)rc.size() > cols() )
			cols((int)rc.size());
	}
	// How many rows we loaded
	rows((int)rowdata.size());

	/* important, after first load dir */
	row_height_all(15);

	// Auto-calculate widths, with 15 pixel padding
	autowidth(15);
}

void table_files::draw_sort_arrow(int X, int Y, int W, int H)
{
	int xlft = X + (W - 6) - 8;
	int xctr = X + (W - 6) - 4;
	int xrit = X + (W - 6) - 0;
	int ytop = Y + (H / 2) - 4;
	int ybot = Y + (H / 2) + 4;

	if (sort_reverse) {
		// Engraved down arrow
		fl_color(FL_WHITE);
		fl_line(xrit, ytop, xctr, ybot);
		fl_color(41); // dark gray
		fl_line(xlft, ytop, xrit, ytop);
		fl_line(xlft, ytop, xctr, ybot);
	} else {
		// Engraved up arrow
		fl_color(FL_WHITE);
		fl_line(xrit, ybot, xctr, ytop);
		fl_line(xrit, ybot, xlft, ybot);
		fl_color(41); // dark gray
		fl_line(xlft, ybot, xctr, ytop);
	}
}

void table_files::draw_cell(TableContext context,
		int R, int C, int X, int Y, int W, int H)
{
	const char *s = "";

	if ( R < (int) rowdata.size() && C < (int)rowdata[R].cols.size() )
		s = rowdata[R].cols[C];

	switch (context) {
	case CONTEXT_COL_HEADER:
		fl_push_clip(X, Y, W, H); {
			fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H,
					FL_BACKGROUND_COLOR);
			if ( C < 9 ) {
				fl_font(font_face_header, font_size_header);
				fl_color(FL_BLUE);
				fl_draw(header[C], X + 2, Y, W, H,
						FL_ALIGN_LEFT, 0, 0);
				// Draw sort arrow
				if (C == 1) {
					draw_sort_arrow(X, Y, W, H);
				}
			}
		}
		fl_pop_clip();
		return;
	case CONTEXT_CELL: {
		fl_push_clip(X, Y, W, H); {
			// Bg color
			Fl_Color bgcolor = row_selected(R) ?
					selection_color() : FL_WHITE;

			if (C == 5) {
				if (strcmp(&s[1], "rwxrwxrwx") == 0)
					bgcolor = FL_YELLOW;
			}

			fl_color(bgcolor);
			fl_rectf(X, Y, W, H);
			fl_font(font_face_row, font_size_row);
			fl_color(15, 15, 15);
			if (C == 0) {
				fl_color(130, 130, 130);
			} else if (C == 1) {
				if (rowdata[R].cols[5][0] == 'd') {
					if (s[0] == '.' && s[1] != 0)
						fl_color(130, 130, 230);
					else {
						fl_font(font_face_row |
							FL_BOLD, font_size_row);
						fl_color(10, 10 , 230);
					}
				} else if (rowdata[R].cols[5][0] == 'l') {
					fl_color(180, 180, 0);
				} else if (s[1] != 0) {
					if (s[0] == '.')
						fl_color(130, 130, 130);
					else
						fl_color(20, 20, 20);
				}
			} else if (C == 6) {
				if (string(s) == current_usr)
					fl_color(0, 165 , 0);
			} else if (C == 7) {
				if (string(s) == current_grp)
					fl_color(0, 165 , 0);
			}

			fl_draw(s, X + 2, Y, W, H, FL_ALIGN_LEFT |
					FL_ALIGN_CENTER);
			// Border
			fl_color(250, 250, 250);
			fl_rect(X, Y, W, H);
		}
		fl_pop_clip();
		return;
	}
	default:
		return;
	}
}
