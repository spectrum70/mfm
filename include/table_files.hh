#ifndef __table_files_hh__
#define __table_files_hh__

#include "globals.hh"
#include "path.hh"
#include <FL/Fl_Table_Row.H>
#include <vector>
#include <string>

using std::vector;
using std::string;

struct row
{
	vector<char*> cols;
};

struct table_files : public path, public Fl_Table_Row
{
	table_files(int x, int y, int w, int h, app &ptrs);

	void load_dir(const char *dir = 0);

	/* file common operations, on selected item */
	void rename();
	void trash(bool folder = false);
	void copy();
	void cut();
	void paste();
	void move_selection_up();
	void move_selection_down();
	void open_file(string &file);

	string get_selected() { return selected; }
private:
	void autowidth(int pad);

	static void __event_callback(Fl_Widget *, void *data)
	{ ((table_files*)data)->event_callback(); }
	static void __handle_rmenu(Fl_Widget *w, void *data)
	{ ((table_files*)data)->handle_rmenu(w); }

	void event_callback();
	void handle_rmenu(Fl_Widget *);
	void draw_cell(TableContext context, int R = 0, int C = 0,
				int X = 0, int Y = 0, int W = 0, int H = 0);
	void draw_sort_arrow(int X, int Y, int W, int H);
	void sort_column(int col, int reverse);

private:
	app &a;
	vector<row> rowdata;
	string selected;
	string clip_op_src;
	string current_usr, current_grp;
	char longname[512];
	int sort_reverse;
	int sort_lastcol;
	int cl_op;
};

#endif /* __table_files_hh__ */
