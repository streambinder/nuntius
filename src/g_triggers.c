#include <gio/gio.h>
#include <unistd.h>

extern void g_worker_webmail(void *params)
{
	const char *command = malloc((strlen("xdg-open") + 1 + strlen(params)) * sizeof(char));
	sprintf(command, "xdg-open %s", params);
	g_print("[g_worker_webmail] status: %d\n", system(command));
}

extern void g_trigger_webmail(GSimpleAction *action, GVariant *parameter, gpointer window)
{
	const gchar *value = g_variant_get_string(parameter, NULL);
	g_print("[g_trigger_webmail] triggered: %s\n", value);
	g_thread_run(g_worker_webmail, value);
}
