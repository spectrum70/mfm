/*
 * mfm
 *
 * (C) 2017 Angelo Dureghello <angelo@sysam.it>
 *
 * This file is part of mfm application.
 *
 * minilibc library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * gasc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nome-Programma.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "win_main.hh"

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Table.H>
#include <FL/Fl_Table_Row.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Widget.H>

#include <FL/fl_draw.H>

#include <cstdio>
#include <vector>
#include <string>
#include <memory>

#include "path.hh"

constexpr int HEADER_FONTFACE = FL_HELVETICA;
constexpr int HEADER_FONTSIZE = 12;
constexpr int ROW_FONTFACE = FL_HELVETICA;
constexpr int ROW_FONTSIZE = 11;

using namespace std;

static const char *header[] =
	{ "", "name", "size", "date", "time", "rights", "owner", "group", 0 };

struct row
{
	vector<char*> cols;
};

struct table : public path, public Fl_Table_Row
{
	table(int x, int y, int w, int h);
private:
	void autowidth(int pad);

protected:
	void draw_cell(TableContext context, int R = 0, int C = 0,
			int X = 0, int Y = 0, int W = 0, int H = 0);
	void draw_sort_arrow(int X, int Y, int W, int H);
	static void event_callback(Fl_Widget*, void*);
	void event_callback2();

public:
	void load_dir();

private:
	vector<row> rowdata;
	int sort_reverse;
	int sort_lastcol;
};

table::table(int x, int y, int w, int h)
: path(), Fl_Table_Row(x, y, w, h), sort_reverse(0), sort_lastcol(0)
{
	row_resize(0);
	// Col init
	col_header(1);
	col_header_height(14);
	col_resize(1);

	selection_color(FL_YELLOW);
	color(FL_WHITE);

	load_dir();
	/* important, after first load dir */
	row_height_all(14);

	callback(event_callback, (void*)this);

	end();
}

void table::autowidth(int pad)
{
	int w, h;

	// Initialize all column widths to header width
	fl_font(HEADER_FONTFACE, HEADER_FONTSIZE);
	for ( int c = 0; header[c]; c++ ) {
		w = 0;
		fl_measure(header[c], w, h, 0);
		col_width(c, w +pad);
	}
	fl_font(ROW_FONTFACE, ROW_FONTSIZE);
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

void table::event_callback(Fl_Widget*, void *data)
{ ((table*)data)->event_callback2(); }

void table::event_callback2()
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
		fl_alert("trulla");
		}
		break;
	case CONTEXT_CELL:
		if (Fl::event() == FL_RELEASE && Fl::event_button() == 1) {
			if (C == 1) {
				if (rowdata[R].cols[5][0] == 'd') {
					update_path(rowdata[R].cols[1]);
					load_dir();
					redraw();
				} else {
					system((string("kate ") +
						fs_path.c_str() + "/" +
						rowdata[R].cols[1]).c_str());
				}
			}
		}
		break;
	}
	default:
	return;
	}
}

void table::load_dir()
{
	char s[512];
	char n[16];

#define COLUMNS  9
#define LS_OUT_COLS 11

	const int remap[LS_OUT_COLS] = {5, 1, 6, 7, 2, 3, 0, 4, 1, 0, 0};

	rowdata.clear();

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
		for(int t = 0 ; (t == 0) ? (ss = strtok(s, delim)):
				(ss = strtok(NULL, delim)); t++) {
			if (!t) {
				sprintf(n, "%d", i);
				rc[0] = strdup(n);
			} else if (t == 5) {
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

	// Auto-calculate widths, with 20 pixel padding
	autowidth(15);
}

void table::draw_sort_arrow(int X, int Y, int W, int H)
{
	int xlft = X+(W-6)-8;
	int xctr = X+(W-6)-4;
	int xrit = X+(W-6)-0;
	int ytop = Y+(H/2)-4;
	int ybot = Y+(H/2)+4;

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

void table::draw_cell(TableContext context,
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
				fl_font(HEADER_FONTFACE, HEADER_FONTSIZE);
				fl_color(FL_BLUE);
				fl_draw(header[C], X + 2, Y, W, H,
						FL_ALIGN_LEFT, 0, 0);
				// Draw sort arrow
				if (C == sort_lastcol) {
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
			fl_font(ROW_FONTFACE, ROW_FONTSIZE);
			fl_color(FL_BLACK);
			if (C == 1) {
				if (rowdata[R].cols[5][0] == 'd')
					fl_color(10, 10 , 230);
				if (s[0] == '.' && s[1] != 0)
					fl_color(180, 180, 180);
			} else if (C == 6)
				fl_color(0, 165 , 0);

			fl_draw(s, X + 2, Y, W, H, FL_ALIGN_LEFT |
					FL_ALIGN_BOTTOM);
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

win_main::win_main()
{
	s.width = 800;
	s.heigth = 600;

	w = make_shared<Fl_Window>(s.width, s.heigth, "mfm");
	t = make_shared<table>(2, 12, s.width - 4, 586);

	t->tooltip("Use mouse or Shift + Arrow Keys to make selections.\n"
			"Sum of selected values is shown.");

	w->resizable(*t);

	w->end();
	w->show();
}

int win_main::run()
{
	return Fl::run();
}
