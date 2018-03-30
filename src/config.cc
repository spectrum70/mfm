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

#include "config.hh"
#include "fs.hh"

#include <string>

using std::string;

static const char default_config[] = "/home/angelo/.mfm";

config::config()
{
	config_init(&cfg);

	root = config_root_setting(&cfg);
	if (!root) {
		fprintf(stderr, "++err: config(), rot is null\n");
	}
}

config::~config()
{
	config_destroy(&cfg);
}

void config::setup_defaults()
{
	fs filesystem;

	string home = "home," + filesystem.get_user_home();

	add_option("application", "width", 800);
	add_option("application", "height", 600);

	add_option("bookmarks", "bm001", "root,/");
	add_option("bookmarks", "bm002", home.c_str());

	save_config();
}

bool config::load_config()
{
	if(!config_read_file(&cfg, default_config))
		return false;

	root = config_root_setting(&cfg);

	return true;
}

config_setting_t* config::get_group(const char *sect)
{
	config_setting_t *group;

	group = config_lookup(&cfg, sect);
	if (!group) {
		group = config_setting_add(root, sect, CONFIG_TYPE_GROUP);
		if (!group) {
			fprintf(stderr, "++err: add_option(), group %s\n",
					sect);
			return 0;
		}
	}

	return group;
}

bool config::add_option(const char *sect, const char *name, const char *value)
{
	config_setting_t *group, *setting;

	group = get_group(sect);

	setting = config_setting_add(group, name, CONFIG_TYPE_STRING);
	if (!setting) {
		fprintf(stderr, "++err: add_option(), name %s\n", name);
		return false;
	}

	config_setting_set_string(setting, value);

	return true;
}

bool config::add_option(const char *sect, const char *name, int value)
{
	config_setting_t *group, *setting;

	group = get_group(sect);

	setting = config_setting_add(group, name, CONFIG_TYPE_INT);
	if (!setting) {
		fprintf(stderr, "++err: add_option(), name %s\n", name);
		return false;
	}

	config_setting_set_int(setting, value);

	return true;
}

int config::get_int(const char *path)
{
	int rval;
	config_setting_t *group, *setting;

	if (config_lookup_int(&cfg, path, &rval) == CONFIG_TRUE)
		return rval;

	return -1;
}

vect_str config::get_string_list(const char *sect)
{
	config_setting_t *group;
	vect_str rvalues;
	char bm[7] = {"bm"};
	const char *location;
	int i, count;

	rvalues.clear();

	group = get_group(sect);
	if (!group)
		fprintf(stderr, "++err: get_string_list()\n");

	count = config_setting_length(group);
	for(i = 0; i < count; ++i) {
		location = config_setting_get_string_elem(group, i);
		rvalues.push_back(location);
	}

	return rvalues;
}

bool config::save_config()
{
	if(!config_write_file(&cfg, default_config)) {
		fprintf(stderr, "++err: while writing file.\n");
		config_destroy(&cfg);

		return false;
	}

	return true;
}
