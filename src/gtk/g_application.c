#include <gio/gio.h>
#include <pthread.h>
#include <stdio.h>

#include "g_trigger.h"

static pthread_t *application;

static pthread_t **workers;
static size_t workers_size = 0;

static void g_activate(GApplication *g_app)
{
	g_application_hold(g_app);
	// init routines
	g_application_release(g_app);
}

static void g_fork(GApplication *g_app)
{
	g_application_run(g_app, NULL, NULL);
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
	pthread_create(&application, NULL, &g_fork, g_app);
}

extern void g_shutdown(GApplication *g_app)
{
	g_application_quit(g_app);
}

extern void g_thread_run(void (*function)(void *), void *params)
{
	pthread_t worker;
	pthread_create(&worker, NULL, function, params);
	workers = realloc(workers, (workers_size + 1) * sizeof(pthread_t));
	workers[workers_size++] = &worker;
	g_printf("[g_thread_run] subscribed workers: %d\n", workers_size);
}