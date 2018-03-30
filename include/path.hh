#ifndef __path_hh__
#define __path_hh__

#include <string>
#include <experimental/filesystem>

using namespace std;

struct path {
	path();
	path(const string &);

	void update_path(const string &name);

	string get_home_path() { return user_home; }
	string get_cur_path() { return fs_path; }
	string get_cur_folder();

protected:
	string user_home;
	experimental::filesystem::path fs_path;
};

#endif /* __path_hh__ */
