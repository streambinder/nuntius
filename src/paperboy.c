#include <ctype.h>
#include <fcntl.h>
#include <getopt.h>
#include <gio/gio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "account.h"
#include "config.h"
#include "g_application.h"
#include "imap.h"
#include "paperboy.h"

static GApplication *g_app;
static char *config_path;

int main(int argc, char *argv[])
{
	int opt = 0;
	while ((opt = getopt(argc, argv, "c:")) != -1) {
		switch (opt) {
		case 'c':
			config_path = optarg;
			break;
		}
	}

	if (config_path == NULL) {
		printf("[paperboy] no configuration file given\n");
		return 1;
	}

	if (access(config_path, F_OK) == -1) {
		printf("[paperboy] %s: no such file or directory\n", config_path);
		return 1;
	}

	config_t *config = config_from_yaml(config_path);
	printf("[paperboy] configuration parsed: %d accounts / %d min scan interval\n",
	       config->accounts_size,
	       config->scan_interval);

	g_app = g_create("it.davidepucci.PaperBoy");
	g_run(g_app);

	for (int i = 0; i < config->accounts_size; i++) {
		int unread = imap_unread(config->accounts[i]->address,
					 config->accounts[i]->password,
					 config->accounts[i]->hostname);
		if (unread == -1) {
			continue;
		}

		printf("[paperboy] %s: %d unread\n", config->accounts[i]->address, unread);

		if (unread > 0) {
			char *notify_title = (char *)calloc(25, sizeof(char *));
			char *notify_body = (char *)calloc(150, sizeof(char *));

			sprintf(notify_title, "Status for %s", config->accounts[i]->address);
			sprintf(notify_body, "%d emails unread", unread);
			g_notify(g_app, notify_title, notify_body, config->accounts[i]->url);
		}
	}

	sleep(10);
	g_shutdown(g_app);
	return 0;
}
