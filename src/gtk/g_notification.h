#ifndef G_NOTIFICATION_H
#define G_NOTIFICATION_H

void g_notify(GApplication *g_app, char *g_notify_title, char *g_notify_body,
	      char *g_notify_callback);

#endif /* G_NOTIFICATION_H */