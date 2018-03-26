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

#include "table_files.hh"
#include "input.hh"

#include <unistd.h>

#include <FL/fl_draw.H>
#include <FL/fl_ask.H>

constexpr int font_face_header = FL_HELVETICA;
constexpr int font_size_header = 11;
constexpr int font_face_row = FL_HELVETICA;
constexpr int font_size_row = 11;

static const char *header[] = {
		"cnt",
		"name",
		"size",
		"date",
		"time",
		"rights",
		"owner",
		"group",
		0
};

table_files::table_files(int x, int y, int w, int h, app &ptrs)
: path(), Fl_Table_Row(x, y, w, h), a(ptrs), sort_reverse(0), sort_lastcol(0)
{
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

void table_files::event_callback()
{
	int R = callback_row();
	int C = callback_col();
	TableContext context = callback_context();

	switch ( context ) {
	case CONTEXT_COL_HEADER: {
		// someone clicked on column header
		if (Fl::event() == FL_RELEASE && Fl::event_button() == 1 ) {
		/*if ( sort_lastcol == COL ) { // Click same column? Toggle sort
		    sort_reverse ^= 1;
		} else { // Click diff column? Up sort
		    sort_reverse = 0;
		}
		//sort_column(COL, _sort_reverse);
		sort_lastcol = COL;*/
		}
		break;
	case CONTEXT_CELL:
		if (Fl::event() == FL_KEYDOWN) {
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

	printf("row_top %d\n", row_top);

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

	printf("row_top %d\n", row_top);

	select_row(row_top - 1, 0);
	select_row(row_top, 1);
	set_selection(row_top, col_left, row_bot, col_right);
}

void table_files::trash()
{
	string text = "delete <" + selected + "> ?";

	if (fl_choice(text.c_str(), "Yes", "No", 0) == 0) {
		unlink((string(fs_path) + "/" + selected).c_str());
	}

	load_dir();
}

void table_files::copy()
{
	//(string(fs_path) + "/" + selected).c_str());
}

void table_files::cut()
{
	//(string(fs_path) + "/" + selected).c_str());
}

void table_files::paste()
{
	//unlink((string(fs_path) + "/" + selected).c_str());
}

void table_files::load_dir(const char *path)
{
	char s[512];
	char n[16];

#define COLUMNS  9
#define LS_OUT_COLS 11

	const int remap[LS_OUT_COLS] = {5, 0, 6, 7, 2, 3, 0, 4, 1, 0, 0};

	rowdata.clear();

	if (path)
		fs_path = path;

	a.i->value(fs_path.c_str());

	strcpy(s, "ls -alh ");
	strcat(s, fs_path.c_str());

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
		rc.resize(COLUMNS);

		// Break the line
		for (int t = 0; t < LS_OUT_COLS; t++) {
			if (t == 0) {
				ss = strtok(s, delim);
				sprintf(n, "%d", i);
				rc[0] = strdup(n);
			} else if (t == 8) {
				longname[0] = 0;
				while (ss = strtok(NULL, delim)) {
					if (longname[0] != 0)
						strcat(longname, " ");
					strcat(longname, ss);
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
			if (C == 1) {
				if (rowdata[R].cols[5][0] == 'd')
					fl_color(10, 10 , 230);
				if (s[0] == '.' && s[1] != 0)
					fl_color(180, 180, 180);
			} else if (C == 6)
				fl_color(0, 165 , 0);

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
