#include <ctype.h>
#include <fcntl.h>
#include <getopt.h>
#include <gio/gio.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "account.h"
#include "config.h"
#include "g_application.h"
#include "g_notification.h"
#include "imap.h"
#include "paperboy.h"

static GApplication *g_app;
static volatile int shutdown = 0;

void sigint_handler()
{
	shutdown = 1;
}

int main(int argc, char *argv[])
{
	int opt = 0;
	int interval = 300;
	char *config_path = NULL;

	while ((opt = getopt(argc, argv, "c:i:")) != -1) {
		switch (opt) {
		case 'c':
			config_path = optarg;
			break;
		case 'i':
			interval = strtoul(optarg, NULL, 10);
			break;
		}
	}

	if (config_path == NULL) {
		fprintf(stderr, "[paperboy] no configuration file given\n");
		return 1;
	}

	if (access(config_path, F_OK) == -1) {
		fprintf(stderr, "[paperboy] %s: no such file or directory\n", config_path);
		return 1;
	}

	config_t *config = config_from_yaml(config_path);
	if (config == NULL) {
		fprintf(stderr, "[paperboy] unable to parse configuration file\n");
		return 1;
	}
	printf("[paperboy] configuration parsed: %d accounts\n", config->accounts_size);
	printf("[paperboy] check interval: %ds\n", interval);

	signal(SIGINT, sigint_handler);
	g_app = g_create(APP_NAME);
	g_run(g_app);

	time_t last_check = 0;
	while (!shutdown) {
		if (time(NULL) - last_check < interval) {
			sleep(0.5l);
			continue;
		}

		last_check = time(NULL);
		for (int i = 0; i < config->accounts_size; i++) {
			int unread = imap_unread(config->accounts[i]);
			if (unread == -1) {
				continue;
			}
			printf("[paperboy] %s: %d unread\n", config->accounts[i]->address, unread);

			if (unread > 0) {
				char *notify_title = (char *)calloc(50, sizeof(char *));
				char *notify_body = (char *)calloc(50, sizeof(char *));
				sprintf(notify_title,
					"%s received new emails",
					config->accounts[i]->alias);
				if (unread > 1) {
					sprintf(notify_body,
						"%d new emails for %s",
						unread,
						config->accounts[i]->address);
				} else {
					sprintf(notify_body,
						"New email for %s",
						config->accounts[i]->address);
				}

				g_notify(APP_NAME,
					 config->accounts[i],
					 notify_title,
					 notify_body,
					 config->accounts[i]->url);
			}
		}
	}

	g_shutdown(g_app);
	return 0;
}
