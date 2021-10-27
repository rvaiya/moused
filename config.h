#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>

#define MAX_NAME_LEN 256
#define MAX_MICE 32
#define MAX_BUTTONS 32
#define DEFAULT_SCROLLMODE_SENSITIVITY .1

#define CONFIG_PATH "/etc/moused.conf"
#define LOCK_FILE "/var/lock/moused.lock"

enum action_type {
	ACTION_DEFAULT = 0,
	ACTION_BUTTON,
	ACTION_BUTTON_TOGGLE,
	ACTION_SET_SCROLLMODE,
	ACTION_SET_SENSITIVITY
};

struct action {
	enum action_type type;
	float val;
};


struct mouse_config {
	char name[256];

	float sensitivity;

	uint8_t scroll_swap_axes;
	uint8_t scroll_invert_axes;
	uint8_t scroll_inhibit_x;
	uint8_t scroll_inhibit_y;
	float scrollmode_sensitivity;
	float scroll_sensitivity;

	struct action buttons[MAX_BUTTONS];
	struct action scroll_down;
	struct action scroll_up;
	struct action scroll_left;
	struct action scroll_right;

	struct mouse_config *next;
};

extern struct mouse_config *configs;
void parse_config_file(const char *path);

#endif
