#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <ctype.h>
#include "config.h"

struct mouse_config *configs = NULL;

static int lnum = 0;

static void err(char *fmt, ...)
{
	va_list lst;
	va_start(lst, fmt);
	fprintf(stderr, "ERROR parsing config file (line %d): ", lnum);
	vfprintf(stderr, fmt, lst);
	fprintf(stderr, "\n");
	va_end(lst);
}

static int parse_kvp(char *line, char **name, char **val)
{
	char *c, *end;

	*name = line;
	c = line;

	while(*c && !isspace(*c) && *c != '=') c++;
	end = c;
	while(*c && isspace(*c)) c++;

	if(*c != '=') {
		err("Invalid key value pair.");
		return -1;
	}

	*end = '\0';
	c++;
	while(*c && isspace(*c)) c++;
	*val = c;

	return 0;
}

static int parse_section(char *line, ssize_t len, char section[256])
{
	if(strlen(line)-2 >= 256) {
		err("mouse name exceeds maximum size");
		return 0;
	}

	if(line[len-1] == ']') {
		line[len-1] = '\0';
		strcpy(section, line+1);
	} else {
		err("Invalid section");
		return 0;
	}

	return 1;
}

void parse_action(const char *val, struct action *action)
{
	size_t len = strlen(val);

	if(strstr(val, "btn") == val) {
		action->type = val[len-1] == 't' ?
			ACTION_BUTTON_TOGGLE :
			ACTION_BUTTON;

		action->val = atof(val+3);
	} else if(!strcmp(val, "scrollon")) {
		action->type = ACTION_SET_SCROLLMODE;
		action->val = 1;
	} else if(!strcmp(val, "scrollt")) {
		action->type = ACTION_SET_SCROLLMODE;
		action->val = 2;
	} else if(!strcmp(val, "scrolloff")) {
		action->type = ACTION_SET_SCROLLMODE;
		action->val = 0;
	} else if(strstr(val, "sensitivity(") == val) {
		action->type = ACTION_SET_SENSITIVITY;
		action->val = atof(val+12);
	} else
		err("%s is not a valid action.", val);
}

void parse_config_file(const char *path)
{
	FILE *fh = fopen(path, "r");
	if(!fh) {
		perror("fopen");
		exit(-1);
	}

	lnum = 0;
	ssize_t len;
	size_t sz = 0;
	char *line = NULL;

	struct mouse_config *cfg = NULL;

	while((len=getline(&line, &sz, fh)) > 0) {
		lnum++;
		char *name, *val;

		line[--len] = '\0';
		while(*line && isspace(line[0])) {
			line++;
			len--;
		}

		switch(line[0]) {
			char section[256];

		case '\0':
			goto next;
			break;
		case '[':
			if(parse_section(line, len, section)) {
				cfg = calloc(1, sizeof(struct mouse_config));
				strcpy(cfg->name, section);
				cfg->sensitivity = 1;
				cfg->scrollmode_sensitivity = DEFAULT_SCROLLMODE_SENSITIVITY;
				cfg->scroll_sensitivity = 1;

				cfg->next = configs;
				configs = cfg;
			}
			break;
		case '#':
			break;
		default:
			if(!cfg) {
				err("File must start with a section.");
				exit(-1);
			}

			if(parse_kvp(line, &name, &val) == 0) {
				if(!strncmp(name, "btn", 3)) {
					int n = atoi(name+3);
					if(n > MAX_BUTTONS || n <= 0) {
						err("Button out of range (must be between %d and %d)", 1, MAX_BUTTONS);
						goto next;
					}

					parse_action(val, &cfg->buttons[n-1]);
				} else if(!strcmp(name, "scrolldown")) {
					parse_action(val, &cfg->scroll_down);
				} else if(!strcmp(name, "scrollup")) {
					parse_action(val, &cfg->scroll_up);
				} else if(!strcmp(name, "scrollleft")) {
					parse_action(val, &cfg->scroll_left);
				} else if(!strcmp(name, "scrollright")) {
					parse_action(val, &cfg->scroll_right);
				} else if(strstr(name, "scroll_invert_axes")) {
					cfg->scroll_invert_axes = atoi(val);
				} else if(strstr(name, "scroll_swap_axes")) {
					cfg->scroll_swap_axes = atoi(val);
				} else if(strstr(name, "scroll_inhibit_x")) {
					cfg->scroll_inhibit_x = atoi(val);
				} else if(strstr(name, "scroll_inhibit_y")) {
					cfg->scroll_inhibit_y = atoi(val);
				} else if(strstr(name, "scroll_sensitivity")) {
					cfg->scroll_sensitivity = atof(val);
				} else if(strstr(name, "scrollmode_sensitivity")) {
					cfg->scrollmode_sensitivity = atof(val) * DEFAULT_SCROLLMODE_SENSITIVITY;
				} else if(strstr(name, "sensitivity")) {
					cfg->sensitivity = atof(val);
				} else {
					err("%s is not a valid key", name);
				}
			}
			break;
		}

next:
		free(line);
		line = NULL;
		sz = 0;
	}

	free(line);
	fclose(fh);
}
