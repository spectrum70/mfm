#ifndef __win_help_hh__
#define __win_help_hh__

#include <FL/Fl_Window.H>
#include <memory>

using namespace std;

class Fl_Text_Buffer;
class Fl_Text_Display;

struct win_help : public Fl_Window
{
	win_help(int width, int height);

private:
	shared_ptr<Fl_Text_Buffer> tb;
	shared_ptr<Fl_Text_Display> td;
};

#endif /* __win_help_hh__ */
