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

#include "path.hh"
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

path::path()
{
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;

	fs_path = homedir;
}

path::path(const string &path)
{
	fs_path = path;
}

void path::update_path(const string &name)
{
	if (name == "..") {
		if (fs_path.has_parent_path())
			fs_path = fs_path.parent_path();
	} else if (name != ".")
		fs_path += "/" + name;
}


