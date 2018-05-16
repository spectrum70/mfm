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
#include "win_input.hh"

#include <algorithm>
#include <sys/types.h>
#include <grp.h>
#include <unistd.h>

#include <FL/fl_draw.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_.H>
#include <FL/Fl_Menu_Item.H>

constexpr int font_face_header = FL_HELVETICA;
constexpr int font_size_header = 11;
constexpr int font_face_row = FL_HELVETICA;
constexpr int font_size_row = 11;
constexpr int max_username_len = 32;

constexpr int COLUMNS = 9;
constexpr int LS_OUT_COLS = 9;

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

	type(Fl_Table_Row::SELECT_SINGLE);

	selection_color(FL_YELLOW);
	color(FL_WHITE);

	when(FL_WHEN_NOT_CHANGED | FL_WHEN_ENTER_KEY);
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
		int R, C;

		get_selection (R, C, R, C);
		trash(rowdata[R].cols[5][0] == 'd');
	} else if (strncmp(mi->label(), "rename", 6) == 0) {
		rename();
	}
}

void table_files::dnd_initiate()
{
	int R = callback_row();
	char *url = new char[512];
	string drag_selected;

	memset(url, 0, 512);

	drag_selected = fs_path.c_str();
	drag_selected += "/";
	drag_selected += rowdata[R].cols[1];

	sprintf(url, "file://%s\r\n",
		  		drag_selected.c_str());
	Fl::copy(url, strlen(url) + 1, 2);

	// At this point FLTK's system wide DnD kicks in and we lose all control
 	Fl::dnd();

	delete [] url;
}

int table_files::handle(int event)
{
	TableContext context = callback_context();

	switch (event) {
	case FL_DRAG:
		/* Do not allow drag on header */
		if (context == CONTEXT_CELL)
			dnd_initiate();
		break;
	case FL_DND_ENTER:
	case FL_DND_RELEASE:
	case FL_DND_LEAVE:
	case FL_DND_DRAG:
	case FL_PASTE:
		break;
	}

	return Fl_Table_Row::handle(event);
}

void table_files::event_callback()
{
	int R = callback_row();
	int C = callback_col();
	TableContext context = callback_context();

	switch (context) {
	case CONTEXT_COL_HEADER:
		// someone clicked on column header
		if (Fl::event() == FL_PUSH && Fl::event_button() == 1) {
			if (sort_lastcol == C) {
				sort_reverse ^= 1;
			} else {
				sort_reverse = 0;
			}
			sort_column(C, sort_reverse);
			sort_lastcol = C;
		}
		return;

	default:
		return;
	case CONTEXT_TABLE:
	case CONTEXT_CELL:
		break;
	}

	/* We are CONTEXT_CELL here */
	switch (Fl::event()) {
	case FL_KEYDOWN:
		switch (Fl::event_key()) {
		case FL_Delete:
			/* key pressed handled here */
			trash();
			break;
		case 'c':
			if (Fl::get_key(FL_Control_L) ||
					Fl::get_key(FL_Control_R)) {
				copy();
			}
			break;
		case 'v':
			if (Fl::get_key(FL_Control_L) ||
				Fl::get_key(FL_Control_R)) {
				paste();
			}
			break;
		case 'z':
			if (Fl::get_key(FL_Control_L) ||
				Fl::get_key(FL_Control_R)) {
				cut();
			}
			break;
		case FL_Up:
			move_selection_up();
			break;
		case FL_Down:
			move_selection_down();
			break;
		case FL_Left:
			if (fs_path != "/")
				update_path("..");
			load_dir();
			/* avoid to lose focus */
			set_selection(1, 1, 1, 1);
			select_row(1, 1);
			break;
		case  FL_Right:
			if (rowdata[R].cols[5][0] == 'd') {
				update_path(rowdata[R].cols[1]);
				load_dir();
			}
			break;
		case  FL_Enter:
			if (rowdata[R].cols[5][0] != 'd')
				open_file(selected);
			else {
				update_path(rowdata[R].cols[1]);
				load_dir();
			}
			focus(this);
			break;
		}
		selected = rowdata[R].cols[1];
		return;

	case FL_PUSH:
		if (Fl::event_button() == 3) {
			select_row(R, 1);
			set_selection(R, 0, R, -1);
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

			return;
		}

		/* Don't handle context table */
		if (context == CONTEXT_TABLE)
			return;

		/* mouse events */
		if (rowdata[R].cols[5][0] == 'd') {
			update_path(rowdata[R].cols[1]);
			load_dir();
		} else if (rowdata[R].cols[5][0] == 'l') {
			update_path(rowdata[R].cols[COLUMNS - 1]);
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
}

void table_files::move_selection_up()
{
	int row_top, col_left, row_bot, col_right;

	get_selection(row_top, col_left, row_bot, col_right);

	if (row_top < 0)
		return;

	select_row(row_top - 1, 0);
	select_row(row_top, 1);
	set_selection(row_top, col_left, row_bot, col_right);
}

void table_files::move_selection_down()
{
	int row_top, col_left, row_bot, col_right;

	get_selection(row_top, col_left, row_bot, col_right);
	select_row(row_top + 1, 0);
	select_row(row_top, 1);
	set_selection(row_top, col_left, row_bot, col_right);
}

void table_files::create_folder()
{
	string new_name;

	win_input i(parent()->x() + 140,
		    parent()->y() + 40, 300, 24, "New folder");

	i.clear_border();
	i.show();

	while (i.shown() && !i.updated())
		Fl::wait();

	i.hide();

	new_name = i.updated_text();
	new_name.resize(new_name.size() - 1);
	new_name = string(fs_path) + "/" + new_name;

	system((string("mkdir ") + new_name).c_str());
	load_dir();
}

string table_files::input_on_selection()
{
	int row_top, col_left, row_bot, col_right;

	get_selection(row_top, col_left, row_bot, col_right);
	int width = col_width(1);

	win_input i(parent()->x() + x(),
		    parent()->y() + (row_bot * (12 + 3)) + 45,
			width + 20, 24,
		    selected);

	i.clear_border();
	i.show();

	while (i.shown() && !i.updated())
		Fl::wait();

	i.hide();

	return i.updated_text();
}

void table_files::rename()
{
	string new_name, src, dst;
	
	new_name = input_on_selection();
	new_name.resize(new_name.size() - 1);

	src = string(fs_path) + "/" + selected;
	dst = string(fs_path) + "/" + new_name;

	src = ls_real(src);

	::rename(src.c_str(), dst.c_str());
	load_dir();
}

void table_files::trash(bool folder)
{
	string text;

	if (folder)
		text = "remove folder [ " + selected +
			"] and all contained files ?";
	else
		text = "delete [ " + selected + " ] ?";

	string target = string(fs_path) + "/" + selected;

	if (fl_choice(text.c_str(), "Yes", "No", 0) == 0) {
		if (folder) {
			path p;

			p.remove_folder(target.c_str());
		} else
			unlink(target.c_str());
	}

	load_dir();
}

void table_files::copy()
{
	int R = callback_row();
	char *url = new char[512];
	string item_selected;

	memset(url, 0, 512);

	item_selected = fs_path.c_str();
	item_selected += "/";
	item_selected += rowdata[R].cols[1];

	/*
	 * FLTK has a limitation here. Whatever is in the clipboard is always
	 * sent as text. So sending the path to a file might not actually send
	 * the file, but just the path and filename. Some apps interprete that
	 * correctly, some don't. On Unix, you may have better luck prepending
	 * the word "file:" to the pathname.
	 */
	sprintf(url, "file://%s\r\n", item_selected.c_str());

	Fl::copy(url, strlen(url), 1);

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
	string cmd, src, dest, src_path;
	unsigned int z;
	Fl_Input in(0, 0, 0, 0, 0);

	Fl::paste(in, 1);

	src = in.value();

	/* Remove std prefix file:// */
	src.erase(0, 7);

	if ((z = src.rfind('/')) != string::npos) {
        	dest = src.substr(z + 1);
		src_path = src.substr(0, z);
	} else
		return;

	if (cl_op == cl_op_copy) {
		fs f;

		if (src_path == fs_path) {
			dest = input_on_selection();
		}

		cmd = "cp ";
		if (f.is_dir(src.c_str()))
			cmd += "-a ";

	} else if (cl_op == cl_op_cut) {
		cmd = "mv ";
	}

	cmd += src + " " + string(fs_path) + "/" + dest;

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
	const int remap[LS_OUT_COLS] = {5, 0, 6, 7, 2, 3, 0, 4, 1};

	rowdata.clear();

	if (path)
		fs_path = path;

	a.i->value(fs_path.c_str());

	strcpy(s, "ls -alhb ");
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
		rc.resize(COLUMNS);

		// Break the line
		for (int t = 0; t < LS_OUT_COLS; t++) {
			if (t == 0) {
				ss = strtok(s, delim);
				sprintf(n, "%d", i);
				rc[0] = strdup(n);
			} else if (t == 8) {
				bool link = false;
				longname[0] = 0;
				while ((ss = strtok(NULL, delim))) {
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
				if (ss)
					strcpy(n, ss);
			} else if (t == 6) {
				if (ss) {
					strcat(n, " ");
					strcat(n, ss);
					rc[remap[5]] = strdup(n);
				}
			}

			if (remap[t] && ss) {
				rc[remap[t]] =
					strdup((char *)
						ls_normalize(ss).c_str());
			}
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
