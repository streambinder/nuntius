#include <gio/gio.h>
#include <stdio.h>
#include <unistd.h>

extern void g_trigger_webmail(GSimpleAction *action, GVariant *parameter, gpointer window)
{
	char *const callback = (char *)g_variant_get_string((GVariant *)parameter, NULL);
	char *command[] = { "xdg-open", callback, NULL };
	g_autoptr(GError) error = NULL;
	g_spawn_async(NULL,
		      command,
		      NULL,
		      G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_SEARCH_PATH,
		      NULL,
		      NULL,
		      NULL,
		      &error);
	if (error != NULL) {
		g_error("[g_trigger_webmail] unable to fork: %s", error->message);
		return;
	}
}