#include <sys/stat.h>

#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>

static GdkPixbuf *	 default_thumbnail(struct dirent *);
static GdkPixbuf *	 try_thumbnail(char *, char *);

extern int	errno;

/*
 * Find the appropriate thumbnail for a dirent in a directory.
 *
 * If the generated thumbnail exists as
 * $XDG_CACHE_HOME/.thumbnails/large/$md5.png then use that. If it does not
 * exist, or if it exists but cannot be turned into a pixbuf, use the default
 * file or directory icons.
 */
GdkPixbuf *
find_thumbnail(struct dirent *dp, char *directory)
{
	GdkPixbuf	*pixbuf;
	char		*digest, *uri;
	int		 len;

	/*    file:// + directory         + / + basename           + \0 */
	len = 7        + strlen(directory) + 1 + strlen(dp->d_name) + 1;
	if ((uri = calloc(len, sizeof(char))) == NULL)
	    err(1, "calloc");
	snprintf(uri, len, "file://%s/%s", directory, dp->d_name);

	digest = g_compute_checksum_for_string(G_CHECKSUM_MD5, uri, -1);
	free(uri);

	pixbuf = try_thumbnail(digest, "large");

	if (pixbuf == NULL)
		pixbuf = try_thumbnail(digest, "normal");

	g_free(digest);

	if (pixbuf == NULL)
		pixbuf = default_thumbnail(dp);

	return pixbuf;
}

/*
 * Try to load the thumbnail for the digest at the given size.
 *
 * RETURN: a GdkPixbuf* on success, and NULL on failure.
 */
static GdkPixbuf *
try_thumbnail(char *digest, char *size)
{
	GdkPixbuf	*pixbuf;
	const gchar	*cache_dir;
	char		*cached_thumb;
	int		 len;
	GError		*error;
	struct stat	*_unused;

	cache_dir = g_get_user_cache_dir();

	/*    cache_dir         + /thumbnails/ + size         + / + digest         + .png + \0 */
	len = strlen(cache_dir) + 12           + strlen(size) + 1 + strlen(digest) + 4    + 1;
	if ((cached_thumb = calloc(len, sizeof(char))) == NULL)
		err(1, "calloc");
	snprintf(cached_thumb, len, "%s/thumbnails/%s/%s.png", cache_dir, size,
	    digest);

	error = NULL;
	errno = 0;
	_unused = NULL;

	if (stat(cached_thumb, _unused) == -1 && errno == ENOENT)
		pixbuf = NULL;
	else if ((pixbuf = gdk_pixbuf_new_from_file_at_scale(cached_thumb, 32, 32, TRUE, &error)) == NULL) {
		warnx("gdk_pixbuf_new_from_file: %s", error->message);
		pixbuf = NULL;
	}

	free(cached_thumb);

	return pixbuf;
}

/*
 * Produce the default file or directory pixbuf, depending on whether we are
 * dealing with a file or a directory.
 */
static GdkPixbuf *
default_thumbnail(struct dirent *dp)
{
	GtkIconTheme	*icon_theme;
	GdkPixbuf	*pixbuf;
	GError		*error;

	error = NULL;
	icon_theme = gtk_icon_theme_get_default();

	pixbuf = gtk_icon_theme_load_icon(
	    icon_theme,
	    /* icon name */ (dp->d_type == DT_DIR ? "folder" : "text-x-generic"),
            /* icon size */ 32,
	    /* flags */ 0,
	    /* error */ &error);
	if (pixbuf == NULL)
		errx(66, "could not load the default pixbuf: %s", error->message);

	return pixbuf;
}
