#ifndef G_NUNTIUS_NOTIFICATION_H
#define G_NUNTIUS_NOTIFICATION_H

#define G_NUNTIUS_NOTIFY_ICON "mail-message-new"

void g_notify(const char *app_name, account_t *account, char *notify_title, char *notify_body,
	      char *url);

#endif /* G_NUNTIUS_NOTIFICATION_H */