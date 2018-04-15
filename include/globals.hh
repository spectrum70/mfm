#ifndef __globals_hh__
#define __globals_hh__

#include <memory>

using std::shared_ptr;

class win_main;
class input;
class toolbar;
class table_locations;
class table_files;
class Fl_Pixmap;
class Fl_RGB_Image;

struct app {
	win_main *wm;
	shared_ptr<input> i;
	shared_ptr<toolbar> tb;
	shared_ptr<table_locations> tl;
	shared_ptr<table_files> tf;
};

namespace graphic {
struct app_icon {
	Fl_RGB_Image *get_application_icon();
private:
	shared_ptr<Fl_Pixmap> xpm;
	shared_ptr<Fl_RGB_Image> icon;
};
}

#endif /* __globals_hh__ */
