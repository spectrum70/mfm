#ifndef __globals_hh__
#define __globals_hh__

#include <memory>

using std::shared_ptr;

class win_main;
class input;
class toolbar;
class table_locations;
class table_files;

struct app {
	win_main *wm;
	shared_ptr<input> i;
	shared_ptr<toolbar> tb;
	shared_ptr<table_locations> tl;
	shared_ptr<table_files> tf;
};

#endif /* __globals_hh__ */
