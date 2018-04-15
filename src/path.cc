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

#include "path.hh"

#include <iostream>

path::path()
{
	string homedir = get_user_home();

	fs_path = homedir;
	user_home = homedir;
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
	} else if (name != ".") {
		if (fs_path != "/")
			fs_path += "/";
		fs_path += name;
	}
}

string path::get_cur_folder()
{
	string rval = "";
	string path = fs_path;
	unsigned int x;

	if ((x = path.rfind('/')) != string::npos)
		rval = path.substr(x + 1);

	return rval;
}

string path::ls_normalize(const string &path)
{
	size_t x;
	unsigned cur = 0;
	string ls_path = path;

	while ((x = ls_path.find(' ', cur)) != string::npos) {
		if (ls_path[x - 1] != '\\')
			ls_path.insert(x, "\\");
		if ((unsigned int)x < ls_path.size())
			cur = x + 1;
		else
			break;
	}

	return ls_path;
}

bool path::remove_folder(const string &path)
{
	std::error_code err_code;

	if (! std::experimental::filesystem::remove_all(path, err_code)) {
		cout << err_code.message() << std::endl;

		return false;
	}

	return true;
}
