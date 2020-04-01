#include <gio/gio.h>

extern void g_t_sample(GSimpleAction *action, GVariant *parameter, gpointer window)
{
	const gchar *value = g_variant_get_string(parameter, NULL);
	g_print("[g_trigger] triggered: %s\n", value);
	char *command = (char *)calloc(150, sizeof(char *));
	sprintf(command, "xdg-open %s", value);
	int status = system(command);
	g_print("[g_trigger] xdg-open status: %d\n", status);
}
