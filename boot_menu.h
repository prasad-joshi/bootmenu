#ifndef __BOOT_MENU_H__
#define __BOOT_MENU_H__

typedef struct be_console {
	int srow;
	int scol;

	int nrows;
	int ncols;

	int cur_row;
	int cur_col;
} be_console_t;

static inline int
cons_total_rows(be_console_t *cons)
{
	return (cons->nrows);
}

static inline int
cons_dispay_rows(be_console_t *cons)
{
	/* leave two rows for box lines */
	return (cons->nrows - 2);
}

static inline int
cons_total_cols(be_console_t *cons)
{
	return (cons->ncols);
}

static inline int
cons_dispay_cols(be_console_t *cons)
{
	/* leave two cols for box lines */
	return (cons->ncols - 2);
}

#define ENTER      10
#define ESC        27

/* Arrow key scan code has ESC, 91, key, enter */
#define ARROW_UP   65
#define ARROW_DOWN 66

/* page up and down scan code has ESC, 91, key, unknow key, enter */
#define PAGE_UP    53
#define PAGE_DOWN  54

/* home and end key scan code contains ESC, 79, key, and enter */
#define HOME       72
#define END        70

#define SORT_ORDER 111 /* 'o' to reverse sort order */
#define SORT_KEY   107 /* 'k' to change sort key    */

#define HELP       104 /* '?' or 'h' to display help */
#define H_KEY      104 /* 'h' key ascii code         */
#define QUST_KEY   63  /* '?' key ascii code         */

int be_menu_select(be_console_t *console, boot_conf_t *conf, boot_env_t **bep,
		int *sort_order, int *sort_key);
int be_console_init(be_console_t *console, int srow, int scol, int nrows, int ncols);

#endif
