#include <gio/gio.h>
#include <pthread.h>
#include <stdio.h>

static void g_activate(GApplication *g_app)
{
	g_application_hold(g_app);
	g_application_release(g_app);
}

static void g_fork(GApplication *g_app)
{
	g_application_run(g_app, 0, NULL);
	g_object_unref(g_app);
}

extern GApplication *g_create(char *g_name)
{
	GApplication *g_app = g_application_new(g_name, G_APPLICATION_HANDLES_OPEN);
	g_signal_connect(g_app, "activate", G_CALLBACK(g_activate), NULL);
	g_application_set_inactivity_timeout(g_app, 10000);
	return g_app;
}

extern void g_run(GApplication *g_app)
{
	pthread_t application;
	pthread_create(&application, NULL, (void *)&g_fork, g_app);
}

extern void g_shutdown(GApplication *g_app, uint delay)
{
	sleep(delay);
	g_application_quit(g_app);
}