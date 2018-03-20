#ifndef __table_files_hh__
#define __table_files_hh__

#include <FL/Fl_Table_Row.H>
#include "path.hh"

#include <vector>

using std::vector;

struct row
{
	vector<char*> cols;
};

struct table_files : public path, public Fl_Table_Row
{
	table_files(int x, int y, int w, int h);
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


#endif /* __table_files_hh__ */
