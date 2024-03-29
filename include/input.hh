#ifndef __input_hh__
#define __input_hh__

#include "globals.hh"
#include <FL/Fl_Input.H>

struct input : public Fl_Input
{
	input(int X, int Y, int W, int H, app &ptrs);

	void cb_input();
protected:
	static void __cb_input(Fl_Widget *w, void *data)
	{ ((input *)data)->cb_input(); }

private:
	app &a;
};

#endif /* __input_hh__ */
