#ifndef __fs_hh__
#define __fs_hh__

#include <string>

using std::string;

struct fs {
	fs();
	string get_user_home();
	bool is_dir(const char *name);
};

#endif /* __fs_hh__ */
