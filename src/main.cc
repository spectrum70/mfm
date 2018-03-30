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

#include "win_main.hh"
#include "config.hh"

int main(int argc, char **argv)
{
	int w, h;

	if (!config::get().load_config())
		config::get().setup_defaults();

	w = config::get().get_int("application.width");
	h = config::get().get_int("application.height");

	win_main wm(w, h);

	return wm.run();
}

