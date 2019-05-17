#include <gio/gio.h>

extern void g_t_sample(GSimpleAction *action,
					GVariant *parameter,
					gpointer window)
{
	const gchar *value = g_variant_get_string(parameter, NULL);
	g_print("triggered: %s\n", value);
}
