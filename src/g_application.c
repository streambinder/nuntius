#include <gio/gio.h>
#include <pthread.h>
#include <stdio.h>

#include "g_triggers.h"

static pthread_t thread_id;
static GActionEntry g_app_entries[] = { { "trigger", g_t_sample, "s", NULL, NULL } };

static void g_activate(GApplication *g_app)
{
	printf("g_activate enter\n");
	g_application_hold(g_app);
	g_action_map_add_action_entries(G_ACTION_MAP(g_app),
					g_app_entries,
					G_N_ELEMENTS(g_app_entries),
					g_app);
	g_application_release(g_app);
	printf("g_activate exit\n");
}

static void *g_fork(GApplication *g_app)
{
	printf("g_fork enter\n");
	int g_status = g_application_run(g_app, NULL, NULL);
	g_object_unref(g_app);
	printf("g_fork exit\n");
}

extern GApplication *g_create(char *g_name)
{
	printf("g_create enter\n");
	GApplication *g_app = g_application_new(g_name, G_APPLICATION_HANDLES_OPEN);
	g_signal_connect(g_app, "activate", G_CALLBACK(g_activate), NULL);
	g_application_set_inactivity_timeout(g_app, 10000);

	printf("g_create exit\n");
	return g_app;
}

extern void g_run(GApplication *g_app)
{
	printf("g_run enter\n");
	pthread_create(&thread_id, NULL, g_fork, g_app);
	// pthread_join(thread_id, NULL);
	printf("g_run exit\n");
}

extern void g_shutdown(GApplication *g_app)
{
	printf("g_shutdown enter\n");
	pthread_exit(0);
	g_application_quit(g_app);
	printf("g_shutdown exit\n");
}

extern void g_notify(GApplication *g_app, char *g_notify_title, char *g_notify_body)
{
	printf("g_notify enter\n");

	g_application_hold(g_app);

	// notification customizing
	GNotification *notification = g_notification_new(g_notify_title);
	g_notification_set_body(notification, g_notify_body);

	// notification actions
	g_notification_set_default_action(notification, "app.go-to-lunch");
	g_notification_add_button_with_target(notification, "Trigger", "app.trigger", "s", "data");

	// notification run
	g_application_send_notification(g_app, "lunch-is-ready", notification);

	g_application_release(g_app);

	printf("g_notify exit\n");
}