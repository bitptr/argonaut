#ifndef DND_H
#define DND_H

enum {
	TARGET_URI_LIST,
	TARGET_COUNT
};

static GtkTargetEntry	dnd_targets[] = {
	{ "text/uri-list", 0, TARGET_URI_LIST }
};

#endif /* DND_H */
