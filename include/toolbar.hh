#ifndef __toolbar_hh__
#define __toolbar_hh__

#include <memory>
#include <FL/Fl_Pack.H>
#include "globals.hh"

using std::shared_ptr;

class Fl_Pixmap;
class toolbar_button;

constexpr int max_buttons = 4;

struct toolbar : public Fl_Pack
{
	toolbar(int X, int Y, int W, int H, app &ptrs);

	void add_button(const char *name, shared_ptr<Fl_Pixmap> &img);

private:
	int handle(int event);

private:
	shared_ptr<toolbar_button> b[max_buttons];
	shared_ptr<Fl_Pixmap> p[max_buttons];

	app &a;
};

#endif /* __toolbar_hh__ */
