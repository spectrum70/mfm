#ifndef __win_main_hh__
#define __win_main_hh__

#include "visual.hh"
#include <memory>

using std::shared_ptr;

class table_files;
class toolbar;
class Fl_Window;

struct win_main {
	win_main();

	int run();
private:
	pos p;
	sz s;

	shared_ptr<Fl_Window> w;
	shared_ptr<table_files> t;
	shared_ptr<toolbar> tb;
};

#endif /* __win_main_hh__ */
