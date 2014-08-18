#ifndef STATE_H
#define STATE_H

#include <gtk/gtk.h>

struct state {
	char		*argv0;		/* How the user calls argonaut */
	char		*dir;		/* The directory */
	GtkIconView	*icon_view;	/* The grid of icons */
	GtkTreePath	*tree_path;	/* The tree path to the drop target */
};

struct state	*state_new(char *);
void		 state_free(struct state *);
int		 state_add_dir(struct state *, char *);

#endif /* STATE_H */
