#include <gio/gio.h>

#include "g_notification.h"
#include "g_trigger.h"

const GActionEntry g_triggers[] = { { "trigger", g_trigger_webmail, "s", NULL, NULL } };

extern void g_notify(GApplication *g_app, char *g_notify_title, char *g_notify_body,
		     char *g_notify_callback)
{
	g_application_hold(g_app);

	g_action_map_add_action_entries(G_ACTION_MAP(g_app),
					g_triggers,
					G_N_ELEMENTS(g_triggers),
					g_app);

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
	g_application_send_notification(g_app, NULL, notification);

	g_application_release(g_app);
}
