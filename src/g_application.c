#include <gio/gio.h>
#include <pthread.h>

#include "g_triggers.h"

static pthread_t thread_id;
static GActionEntry g_app_entries[] = {{"trigger", g_t_sample, "s", NULL, NULL}};

static void g_activate(GApplication *g_app)
{
	g_application_hold(g_app);

	g_action_map_add_action_entries(G_ACTION_MAP(g_app),
									g_app_entries, G_N_ELEMENTS(g_app_entries),
									g_app);

	// notification customizing
	GNotification *notification = g_notification_new("Lunch is ready");
	g_notification_set_body(notification, "Today we have pancakes and salad, and fruit and cake for dessert");

	// notification actions
	g_notification_set_default_action(notification, "app.go-to-lunch");
	g_notification_add_button_with_target(notification, "Trigger", "app.trigger", "s", "data");

	// notification run
	g_application_send_notification(g_app, "lunch-is-ready", notification);

	g_application_release(g_app);
}

static void *g_fork(GApplication *g_app)
{
	printf("g_fork\n");
	int g_status = g_application_run(g_app, NULL, NULL);
	g_object_unref(g_app);
}

extern GApplication *g_create(char *g_name)
{
	printf("g_create\n");
	GApplication *g_app = g_application_new(g_name, G_APPLICATION_HANDLES_OPEN);
	g_signal_connect(g_app, "activate", G_CALLBACK(g_activate), NULL);
	g_application_set_inactivity_timeout(g_app, 10000);

	return g_app;
}

extern void g_run(GApplication *g_app)
{
	printf("g_run\n");
	pthread_create(&thread_id, NULL, g_fork, g_app);
	pthread_join(thread_id, NULL);
}
extern void g_shutdown(GApplication *g_app)
{
	printf("g_shutdown\n");
	pthread_exit(0);
	g_application_quit(g_app);
}