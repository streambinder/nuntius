#include <gio/gio.h>
#include <unistd.h>

extern void g_worker_webmail(void *parameter)
{
	const char *callback = (char *)g_variant_get_string((GVariant *)parameter, NULL);
	const char *command = malloc((strlen("xdg-open") + 1 + strlen(callback)) * sizeof(char));

	if (strlen(callback) == 0) {
		g_print("[g_worker_webmail] callback malformed\n");
		return;
	}

	sprintf(command, "xdg-open %s", callback);
	g_print("[g_worker_webmail] \"%s\" status: %d\n", command, system(command));

	free(command);
	free(callback);
}

extern void g_trigger_webmail(GSimpleAction *action, GVariant *parameter, gpointer window)
{
	g_print("[g_trigger_webmail] triggered\n");
	g_thread_run(g_worker_webmail, parameter);
}