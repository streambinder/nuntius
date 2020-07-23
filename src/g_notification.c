#include <libnotify/notify.h>

#include "account.h"
#include "g_notification.h"

static void g_notify_callback(NotifyNotification *notification, char *action, gpointer data)
{
	g_app_info_launch_default_for_uri_async(data, NULL, NULL, NULL, NULL);
	g_object_unref(G_OBJECT(notification));
}

extern void g_notify(const char *app_name, account_t *account, char *notify_title,
		     char *notify_body, char *url)
{
	if (!notify_is_initted() && !notify_init(app_name)) {
		g_print("[g_notify] unable to init notification engine\n");
	}

	NotifyNotification *n =
	    notify_notification_new(notify_title, notify_body, G_PAPERBOY_NOTIFY_ICON);
	notify_notification_set_app_name(n, app_name);
	if (account->notification_id > 0) {
		g_object_set(n, "id", account->notification_id, NULL);
	}
	notify_notification_add_action(n,
				       "webmail",
				       "Webmail",
				       NOTIFY_ACTION_CALLBACK(g_notify_callback),
				       (gpointer *)url,
				       NULL);
	notify_notification_set_timeout(n, NOTIFY_EXPIRES_DEFAULT);
	notify_notification_show(n, NULL);

	g_object_get(n, "id", &account->notification_id, NULL);
	g_print("[g_notify] notification id: %d\n", account->notification_id);

	free(notify_title);
	free(notify_body);
}
