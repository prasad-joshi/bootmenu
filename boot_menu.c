#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "bootenv.h"
#include "boot_menu.h"

static void
move(int row, int col)
{
	printf("\033[%d;%dH",row,col);
}

static void
erase(int row)
{
	printf("\033[2K");
}

static void
reverse(void)
{
	printf("\033[7m");
}

static void
bold(void)
{
	printf("\033[1m\033[4m");
}

static void
normal(void)
{
	printf("\033[0m");
}

static void
print_msg(be_console_t *cons, char *msg, int msglen)
{
	int nchars;
	int i;

	nchars = cons_dispay_cols(cons);
	for (i = 0; i < nchars && i < msglen; i++) {
		printf("%c", msg[i]);
	}
}

static void
print_normal(be_console_t *cons, char *msg, int msglen, int select)
{
	char *s;
	int  i;

	normal();
	printf("|");
	if (select) {
		reverse();
	}

	print_msg(cons, msg, msglen);

	normal();
	move(cons->cur_row, cons->scol + cons_total_cols(cons));
	printf("|");
}

static void
print_bold(be_console_t *cons, char *msg, int msglen, int select)
{
	char *s;

	normal();
	printf("|");
	if (select) {
		reverse();
	}
	bold();

	print_msg(cons, msg, msglen);

	normal();
	move(cons->cur_row, cons->scol + cons_total_cols(cons));
	printf("|");
}

static void
draw_line(int row, int col, int nchars)
{
	int i;

	move(row, col);
	normal();
	for (i = 0; i < nchars; i++) {
		printf("=");
	}
	normal();
	printf("\n");
}

static void
draw_container(int row, int col, int nchars)
{
	move(row, col);
	normal();
	printf("|");

	move(row, col+nchars);
	printf("|");
}

static void
draw_box(be_console_t *cons)
{
	int sr;
	int sc;
	int er;

	sr = cons->srow;
	sc = cons->scol;
	er = sr + cons_total_rows(cons);

	while (sr <= er) {
		if (sr == cons->srow || sr == er) {
			draw_line(sr, sc, cons_total_cols(cons));
		} else {
			draw_container(sr, sc, cons_total_cols(cons));
		}

		sr++;
	}

	cons->cur_row++;
}

static int
get_key_event(void)
{
	int i;
	int rc;

	rc = getchar();

	if (rc == ENTER) {
		return ENTER;
	} else if (rc == ESC) {
		/* read 91 */
		rc = getchar();

		if (rc == 91) {
			/* read arrow key */
			rc = getchar();
			switch (rc) {
			case ARROW_DOWN:
			case ARROW_UP:
				/* read enter */
				getchar();
				break;
			case PAGE_UP:
			case PAGE_DOWN:
				/* read enter */
				getchar();
				getchar();
			}
		} else if (rc == 79) {
			rc = getchar();
			switch (rc) {
			case HOME:
				getchar();
				break;
			case END:
				getchar();
				break;
			}
		}

		return rc;
	} else if (rc == SORT_ORDER || rc == SORT_KEY) {
		getchar();
		return (rc);
	} else if (rc == H_KEY || rc == QUST_KEY) {
		getchar();
		return (HELP);
	}

	return (0);
}

static int
be_menu_display(be_console_t *cons, boot_conf_t *conf, int skip, int cur_active)
{
	int        i;
	boot_env_t *b;
	char       str[128];
	int        select;
	int        d;        /* number of BEs displayed */

	i = 0;
	d = 0;
	BOOTENV_FOREACH(conf, b) {
		if (i < skip) {
			i++;
			continue;
		}

		if (d > cons_dispay_rows(cons)) {
			break;
		}

		memset(str, 0, sizeof(str));
		bootenv_string(b, str, cons_dispay_cols(cons));

		select = (i == cur_active);

		move(cons->cur_row, cons->cur_col);
		erase(cons->cur_row);
		if (b->active == 0) {
			print_normal(cons, str, strlen(str), select);
		} else {
			print_bold(cons, str, strlen(str), select);
		}

		cons->cur_row++;
		i++;
		d++;
	}

	return (0);
}

struct help_msg {
	char *msg;
	int  is_help;
};

static void
be_menu_display_help(be_console_t *cons, boot_conf_t *conf, int help)
{
	struct help_msg help_msg[] = {
		{"Press 'h' or '?' to display help message.", 0},
		{"Press 'o' to change order of sorting", 1},
		{"Press 'k' to change sort key", 1},
	};

	int             r;
	int             c;
	char            msg[128];
	size_t          sz;
	struct help_msg *h;
	int             i;
	int             n;

	r = cons->nrows + cons->srow + 2;
	c = cons->scol;

	/* print sort key and order information */
	memset(msg, 0, sizeof(msg));
	sz = sizeof(msg);
	strncpy(msg, "BEs sorted using ", sz);

	sz = sizeof(msg) - strlen(msg);
	switch (conf->key) {
	case SORT_OBJNUM:
		strncat(msg, "'object number' ", sz);
		break;
	case SORT_TIMESTAMP:
		strncat(msg, "'creation timestamp' ", sz);
		break;
	case SORT_NAME:
		strncat(msg, "'BE name' ", sz);
		break;
	}

	sz = sizeof(msg) - strlen(msg);
	switch (conf->order) {
	case SORT_ASCENDING:
		strncat(msg, "in ascending order", sz);
		break;
	case SORT_DESCENDING:
		strncat(msg, "in descending order", sz);
		break;
	}

	normal();
	move(r, c);
	erase(r);
	print_msg(cons, msg, strlen(msg));

	/* print help message if required */
	r++;
	r++;

	n = sizeof(help_msg) / sizeof(help_msg[0]);
	for (i = 0; i < n; i++) {
		h = help_msg + i;
		move(r + i, c);
		erase(r + i);
		if (help == 0 && h->is_help == 1) {
			continue;
		}

		print_msg(cons, h->msg, strlen(h->msg));
	}
}

int
be_menu_select(be_console_t *cons, boot_conf_t *conf, boot_env_t **bepp,
	int *sort_order, int *sort_key)
{
	int        rc;
	boot_env_t *b;
	int        cur_active;
	int        skip;
	int        be_count;
	int        be_selected;
	int        help;

	/* NOTE: conf->count does not start with 0 */
	be_count   = conf->count;
	cur_active = 0;
	BOOTENV_FOREACH(conf, b) {
		if (b->active == 0) {
			cur_active++;
			continue;
		}
		break;
	}

	help = 0;
	while (1) {
		skip          = 0;
		cons->cur_row = cons->srow;
		cons->cur_col = cons->scol;

		draw_box(cons);

		be_menu_display_help(cons, conf, help);
		help = 0;

		if (cur_active > cons_dispay_rows(cons)) {
			skip = cur_active - cons_dispay_rows(cons);
		}

		rc = be_menu_display(cons, conf, skip, cur_active);
		if (rc < 0) {
			return (rc);
		}

		rc = get_key_event();
		switch (rc) {
		case ENTER:
			goto enter;
		case SORT_ORDER:
			*bepp       = NULL;
			*sort_order = 1;
			goto out;
		case SORT_KEY:
			*bepp       = NULL;
			*sort_key   = 1;
			goto out;
		case ARROW_UP:
			if (cur_active) {
				cur_active--;
			}
			break;
		case ARROW_DOWN:
			if (cur_active < be_count - 1) {
				cur_active++;
			}
			break;
		case PAGE_UP:
			if (cur_active > cons_dispay_rows(cons)) {
				cur_active -= cons_dispay_rows(cons);
			} else {
				cur_active = 0;
			}
			break;
		case PAGE_DOWN:
			if (cur_active + cons_dispay_rows(cons) < be_count - 1) {
				cur_active += cons_dispay_rows(cons);
			} else {
				cur_active = be_count - 1;
			}
			break;
		case  HOME:
			cur_active = 0;
			break;
		case END:
			cur_active = be_count - 1;
			break;
		case HELP:
			help = 1;
			break;
		}
	}

enter:
	be_selected = cur_active;
	cur_active  = 0;
	b           = NULL;
	BOOTENV_FOREACH(conf, b) {
		if (cur_active == be_selected) {
			break;
		}
		cur_active++;
	}

	assert(b != NULL);
	*bepp = b;

out:
	return (0);
}

int
be_console_init(be_console_t *console, int srow, int scol, int nrows, int ncols)
{
	console->srow  = srow;
	console->scol  = scol;
	console->nrows = nrows;
	console->ncols = ncols;

	return (0);
}
