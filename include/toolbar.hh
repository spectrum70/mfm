#ifndef __toolbar_hh__
#define __toolbar_hh__

#include <memory>
#include <FL/Fl_Pack.H>
#include "globals.hh"

using std::shared_ptr;

class Fl_Pixmap;
class Fl_Box;
class toolbar_button;

enum {
	id_trash,
	id_copy,
	id_cut,
	id_paste,
	id_user_home,
	id_refresh,
	id_arrow_up,
	id_folder_new,
	id_bookmark_new,
	id_help,
	id_about,
	id_max,
};

constexpr int max_seps = 5;

struct toolbar : public Fl_Pack
{
	toolbar(int X, int Y, int W, int H, app &ptrs);

	void add_button(const char *name, shared_ptr<Fl_Pixmap> &img);
	void add_separator();
private:
	int handle(int event);

private:
	shared_ptr<toolbar_button> b[id_max];
	shared_ptr<Fl_Pixmap> p[id_max];
	shared_ptr<Fl_Box> seps[max_seps];

	app &a;
};

#endif /* __toolbar_hh__ */
