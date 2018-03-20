#ifndef __path_hh__
#define __path_hh__

#include <string>
#include <experimental/filesystem>

using namespace std;

struct path {
	path();
	path(const string &);

	void update_path(const string &name);

protected:
	experimental::filesystem::path fs_path;
};



#endif /* __path_hh__ */
