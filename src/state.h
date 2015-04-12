#ifndef STATE_H
#define STATE_H

#include <gtk/gtk.h>

struct state {
	char		*argv0;		/* How the user calls argonaut */
	char		*dir;		/* The directory */
	GtkIconView	*icon_view;	/* The grid of icons */
	GtkTreePath	*tree_path;	/* The tree path to the drop target */
	char		**selected_uris;/* The URIs of selected items */
	pid_t		 pid;		/* A PID to notify */
	int		 in_drag;	/* Are we in the middle of a drag */
};

struct state	*state_new(char *);
void		 state_free(struct state *);
int		 state_add_dir(struct state *, char *);
void		 state_add_known_pid(struct state *, pid_t);

#endif /* STATE_H */
