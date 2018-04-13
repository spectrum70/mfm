#ifndef __win_input_hh__
#define __win_input_hh__

#include <FL/Fl_Window.H>
#include <memory>
#include <string>

using namespace std;

class Fl_Text_Buffer;
class text_editor;

struct win_input : public Fl_Window
{
	win_input(int x, int y, int width, int height, const string &text);

	int handle_key();
private:
	shared_ptr<Fl_Text_Buffer> tb;
	shared_ptr<text_editor> te;

public:
	int updated();
	string updated_text();
};

#endif /* __win_input_hh__ */
