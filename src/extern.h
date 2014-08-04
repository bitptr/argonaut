/* vim: set sw=0 ts=8 cinoptions=:0,t0,+4,(4: */

#ifndef EXTERN_H
#define EXTERN_H


enum model_storage {
	FILE_NAME,
	FILE_ICON,
	FILE_PARENT,
	FILE_TYPE
};

struct cb_data {
	char	*argv0;		/* How the user calls argonaut */
};

struct cb_data	*cb_data_new(char *);
void		 cb_data_free(struct cb_data *);

#endif /* EXTERN_H */
