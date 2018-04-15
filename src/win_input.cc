/*
 * mfm
 *
 * (C) 2017 Angelo Dureghello <angelo@sysam.it>
 *
 * This file is part of mfm application.
 *
 * mfm app. is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * mfm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with mfm.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "win_input.hh"
#include "version.hh"

#include <FL/Fl.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Editor.H>
#include <globals.hh>

constexpr int font_face_row = FL_HELVETICA;
constexpr int font_size_row = 11;

struct text_editor : public Fl_Text_Editor
{
	text_editor(int x, int y, int w, int h) :
	Fl_Text_Editor(x, y, w, h), upd(false)
	{
		when(FL_WHEN_ENTER_KEY_ALWAYS | FL_WHEN_CHANGED);
		maybe_do_callback();
	}

private:
	int handle(int e);

public:
	bool upd;
	string updated_text;
};

int text_editor::handle(int e)
{
	if (Fl::event() == FL_KEYDOWN) {
		switch (Fl::event_key()) {
		case FL_Enter:
			upd = true;
			updated_text = buffer()->text();
			break;
		}
	}

	return Fl_Text_Editor::handle(e);
}

win_input::win_input(int x, int y, int w, int h, const string &text)
: Fl_Window(w, h, "")
{
	tb = make_shared<Fl_Text_Buffer>();
	te = make_shared<text_editor>(1, 1, w - 2, h - 2);

	position(x, y);

	te->buffer(tb.get());
	te->textsize(12);
	te->textfont(font_face_row);
	te->textsize(font_size_row);

	tb->append(text.c_str());
}

int win_input::updated() { return te->upd; }
string win_input::updated_text() { return te->updated_text; }
