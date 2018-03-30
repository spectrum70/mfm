#ifndef __config_hh__
#define __config_hh__

#include <libconfig.h>
#include <vector>
#include <string>

using std::vector;
using std::string;

typedef vector<string> vect_str;

class config {
	config();
	~config();
public:
	static config &get() {
		static config c;

		return c;
	}

	bool add_option(const char *sect, const char *name, const char *value);
	bool add_option(const char *sect, const char *name, int value);
	bool load_config();
	bool save_config();
	void setup_defaults();

	int get_int(const char *path);

	vect_str get_string_list(const char *sect);

private:
	config_setting_t* get_group(const char *sect);
private:
	config_t cfg;
	config_setting_t *root;
};

#endif /* __config_hh__ */
