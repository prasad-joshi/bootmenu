#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bootenv.h"
#include "boot_menu.h"

#define MAX_COLS    80

boot_conf_t  be_conf;
be_console_t console;

static void
genname(char *name, size_t size)
{
	int  c;
	int  i;
	char a;

	c = random() % (size - 1) + 1;
	for (i = 0; i < c - 1; i++) {
		a = 'a' + (random() % 26);
		name[i] = a;
	}
	name[c] = 0;
}

static int
bootenv_random_fill(int count)
{
	int        rc;
	boot_env_t *bep;
	int        i;
	char       name[8];
	uint64_t   objnum;
	uint64_t   timestamp;

	bep = NULL;
	rc  = bootenv_new("mytank/ROOT/workspace", 300, 9101, 0, &bep);
	if (rc < 0) {
		return (-rc);
	}
	rc = bootenv_add(&be_conf, bep);
	if (rc < 0) {
		return (-rc);
	}

	rc  = bootenv_new("abctank/ROOT/vanila", 200, 9101, 1, &bep);
	if (rc < 0) {
		return (-rc);
	}
	rc = bootenv_add(&be_conf, bep);
	if (rc < 0) {
		return (-rc);
	}

	for (i = 0; i < count; i++) {
		genname(name, sizeof(name));
		objnum    = random();
		timestamp = random();

		bep = NULL;
		rc  = bootenv_new(name, objnum, timestamp, 0, &bep);
		if (rc < 0) {
			return (-rc);
		}
		rc = bootenv_add(&be_conf, bep);
		if (rc < 0) {
			return (-rc);
		}
	}

	return (0);
}

int
main(void)
{
	int        rc;
	boot_env_t *bep;
	int        cur_order;
	int        cur_key;
	int        order;
	int        key;
	char       str[512];

	rc        = be_console_init(&console, 5, 5, 10, MAX_COLS - 5);
	cur_order = SORT_ASCENDING;
	cur_key   = SORT_NAME;

	while (1) {
		bep = NULL;
		rc  = bootenv_init(&be_conf, cur_key, cur_order);
		if (rc < 0) {
			return (-rc);
		}

		/* add some dummy boot environments */
		rc = bootenv_random_fill(16);
		if (rc < 0) {
			return (-rc);
		}

		/* display menu */
		order = 0;
		key   = 0;
		rc    = be_menu_select(&console, &be_conf, &bep, &order, &key);
		if (rc < 0) {
			break;
		}

		if (bep != NULL) {
			break;
		} else if (order == 1) {
			if (cur_order == SORT_ASCENDING) {
				cur_order = SORT_DESCENDING;
			} else {
				cur_order = SORT_ASCENDING;
			}
		} else if (key == 1) {
			if (cur_key == SORT_NAME) {
				cur_key    = SORT_TIMESTAMP;
			} else if (cur_key == SORT_TIMESTAMP) {
				cur_key    = SORT_OBJNUM;
			} else if (cur_key == SORT_OBJNUM) {
				cur_key    = SORT_NAME;
			}
		}
	}

	if (bep != NULL) {
		bootenv_string(bep, str, sizeof(str) - 1);
		printf("selected be = %s\n", str);
	}
	return (0);
}
