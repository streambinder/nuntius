#include <gio/gio.h>
#include <stdio.h>
#include <unistd.h>

extern void g_trigger_webmail(GSimpleAction *action, GVariant *parameter, gpointer window)
{
	const char *callback = (char *)g_variant_get_string((GVariant *)parameter, NULL);
	char *command = malloc((strlen("xdg-open") + 1 + strlen(callback)) * sizeof(char));
	sprintf(command, "xdg-open %s", callback);
	g_print("[g_trigger_webmail] \"%s\" status: %d\n", command, system(command));
	free(command);
}