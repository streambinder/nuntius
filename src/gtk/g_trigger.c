#include <gio/gio.h>
#include <stdio.h>
#include <unistd.h>

extern void g_trigger_webmail(GSimpleAction *action, GVariant *parameter, gpointer window)
{
	char *const callback = (char *)g_variant_get_string((GVariant *)parameter, NULL);
	char *const command[] = { "xdg-open", callback, NULL };

	switch (fork()) {
	case -1:
		g_print("[g_trigger_webmail] unable to fork\n");
		break;
	case 0:
		execvp(command[0], command);
		break;
	default:
		break;
	}
}