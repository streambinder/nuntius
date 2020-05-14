#include <libnotify/notify.h>

#include "g_notification.h"

static void g_notify_callback(NotifyNotification *notification, char *action, gpointer data)
{
	char *command[] = { "xdg-open", data, NULL };
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
		g_error("[g_notify_callback] unable to fork: %s", error->message);
		return;
	}
}

extern void g_notify(const char *app_name, char *notify_title, char *notify_body, char *url)
{
	if (!notify_is_initted()) {
		notify_init(app_name);
	}

	NotifyNotification *n =
	    notify_notification_new(notify_title, notify_body, G_PAPERBOY_NOTIFY_ICON);
	notify_notification_add_action(n,
				       "webmail",
				       "Webmail",
				       NOTIFY_ACTION_CALLBACK(g_notify_callback),
				       (gpointer *)url,
				       NULL);
	notify_notification_set_timeout(n, G_PAPERBOY_NOTIFY_TIMEOUT);
	notify_notification_show(n, NULL);

	free(notify_title);
	free(notify_body);
}
