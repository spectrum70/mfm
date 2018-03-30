#ifndef __win_about_hh__
#define __win_about_hh__

#include <FL/Fl_Window.H>
#include <memory>

using namespace std;

class Fl_Text_Buffer;
class Fl_Text_Display;

struct win_about : public Fl_Window
{
	win_about(int width, int height);

private:
	shared_ptr<Fl_Text_Buffer> tb;
	shared_ptr<Fl_Text_Display> td;
};

#endif /* __win_about_hh__ */
