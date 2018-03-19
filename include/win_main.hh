#ifndef __win_main_hh
#define __win_main_hh

#include "visual.hh"
#include <memory>

using namespace std;

class table;
class Fl_Window;

struct win_main {
	win_main();

	int run();
private:
	pos p;
	sz s;

	shared_ptr<Fl_Window> w;
	shared_ptr<table> t;
};

#endif /* __win_main_hh */
