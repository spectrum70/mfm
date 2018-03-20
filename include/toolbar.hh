#ifndef __toolbar_hh__
#define __toolbar_hh__

#include <memory>
#include <FL/Fl_Pack.H>

using std::shared_ptr;

class Fl_Pixmap;
class Fl_Button;

constexpr int max_buttons = 4;

struct toolbar : public Fl_Pack
{
	toolbar(int X,int Y,int W,int H);

	void add_button(const char *name, shared_ptr<Fl_Pixmap> &img,
			Fl_Callback *cb = 0, void *data = 0);
private:
	shared_ptr<Fl_Button> b[max_buttons];
	shared_ptr<Fl_Pixmap> p[max_buttons];
};

#endif /* __toolbar_hh__ */
