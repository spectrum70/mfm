#ifndef __toolbar_hh__
#define __toolbar_hh__

#include <memory>
#include <FL/Fl_Pack.H>
#include "globals.hh"

using std::shared_ptr;

class Fl_Pixmap;
class Fl_Box;
class toolbar_button;

constexpr int max_buttons = 10;
constexpr int max_seps = 4;

struct toolbar : public Fl_Pack
{
	toolbar(int X, int Y, int W, int H, app &ptrs);

	void add_button(const char *name, shared_ptr<Fl_Pixmap> &img);
	void add_separator();
private:
	int handle(int event);

private:
	shared_ptr<toolbar_button> b[max_buttons];
	shared_ptr<Fl_Pixmap> p[max_buttons];
	shared_ptr<Fl_Box> seps[max_seps];

	app &a;
};

#endif /* __toolbar_hh__ */
