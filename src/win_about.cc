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

#include "win_about.hh"

#include <FL/Fl.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Display.H>
#include <globals.hh>

win_about::win_about(int w, int h) : Fl_Window(w, h, "mfm help")
{
	tb = make_shared<Fl_Text_Buffer>();
	td = make_shared<Fl_Text_Display>(1, 1, w - 2, h - 2);

	td->buffer(tb.get());
	td->textfont(FL_COURIER);
	td->textsize(12);

	tb->append("mfm\n\n"
		"(C) 2017 Angelo Dureghello <angelo@sysam.it>\n"
		"\n"
		"mfm is distributed in the hope that it will be useful,\n"
		"but WITHOUT ANY WARRANTY; without even the implied warranty\n"
		"of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
		"See the GNU General Public License for more details.\n"
		"\n"
		"Please see <http://www.gnu.org/licenses/>\n\n");
}
