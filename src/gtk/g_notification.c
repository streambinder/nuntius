#include <gio/gio.h>

#include "g_notification.h"
#include "g_trigger.h"

extern void g_notify(GApplication *g_app, char *g_notify_id, char *g_notify_title,
		     char *g_notify_body, char *g_notify_callback)
{
	// notification customizing
	GNotification *notification = g_notification_new(g_notify_title);
	g_notification_set_body(notification, g_notify_body);
	// notification actions
	g_notification_add_button_with_target(notification,
					      "Webmail",
					      "app.trigger",
					      "s",
					      g_notify_callback);

	// notification run
	g_application_withdraw_notification(g_app, (gchar *)strdup(g_notify_id));
	g_application_send_notification(g_app, (gchar *)strdup(g_notify_id), notification);

	g_dbus_connection_flush_sync(g_application_get_dbus_connection(g_app), NULL, NULL);
	g_object_unref(notification);
}
