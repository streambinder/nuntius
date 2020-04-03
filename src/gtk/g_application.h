#ifndef G_PAPERBOY_APPLICATION_H
#define G_PAPERBOY_APPLICATION_H

GApplication *g_create(char *g_name);
void g_run(GApplication *g_app);
void g_shutdown(GApplication *g_app);
void g_thread_run(void (*function)(void *), void *params);

#endif /* G_PAPERBOY_APPLICATION_H */