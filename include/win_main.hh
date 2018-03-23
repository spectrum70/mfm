#ifndef __win_main_hh__
#define __win_main_hh__

#include "visual.hh"
#include "globals.hh"
#include <memory>
#include <FL/Fl_Window.H>

using std::shared_ptr;

class table_locations;
class table_files;
class toolbar;
class input;

struct win_main : public Fl_Window
{
	win_main();

	int run();

private:
	pos p;
	sz s;
	app ptrs;
};

#endif /* __win_main_hh__ */
