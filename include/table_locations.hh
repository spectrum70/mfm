#ifndef __table_locations_hh__
#define __table_locations_hh__

#include "FL/Fl_Table_Row.H"
#include "globals.hh"
#include <string>
#include <vector>
#include <memory>

class input;

using std::string;
using std::vector;
using std::pair;

struct table_locations : public Fl_Table_Row
{
	table_locations(int x, int y, int w, int h, app &ptrs);

protected:
	void load_locations();
	void draw_cell(TableContext context, int R = 0, int C = 0,
			int X = 0, int Y = 0, int W = 0, int H = 0);

	static void __event_callback(Fl_Widget*, void*);
	void event_callback();

private:
	typedef pair<string, string> loc_path;
	vector<loc_path> locations;
	app &a;
};

#endif /* __table_locations_hh__ */
