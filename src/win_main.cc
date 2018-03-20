/*
 * mfm
 *
 * (C) 2017 Angelo Dureghello <angelo@sysam.it>
 *
 * This file is part of mfm application.
 *
 * mfm library is free software: you can redistribute it and/or modify
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

#include "win_main.hh"
#include "table_files.hh"
#include "toolbar.hh"

#include <FL/Fl_Window.H>

constexpr int window_border = 2;
constexpr int command_bar_height = 30;

win_main::win_main()
{
	s.width = 800;
	s.heigth = 600;

	w = make_shared<Fl_Window>(s.width, s.heigth, "mfm");

	w->begin();
	t = make_shared<table_files>(window_border, command_bar_height + 2,
				s.width - (window_border * 2),
				s.heigth - command_bar_height
				- 2 - window_border);

	w->resizable(*t);

	tb = make_shared<toolbar>(0, 0, w->w(), 28);

	w->end();
	w->show();
}

int win_main::run()
{
	return Fl::run();
}
