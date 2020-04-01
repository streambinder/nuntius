#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <gio/gio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "account.h"
#include "config.h"
#include "g_application.h"
#include "paperboy.h"

static GApplication *g_app;
static char *config_path;

static char *first_match(regex_t *r, const char *to_match)
{
	/* "N_matches" is the maximum number of matches allowed. */
	const int n_matches = 10;
	/* "M" contains the matches found. */
	regmatch_t m[n_matches];
	int i = 0;
	int nomatch = regexec(r, to_match, n_matches, m, 0);
	if (nomatch) {
		printf("No more matches.\n");
		return nomatch;
	}
	for (i = 1; i < n_matches; i++) {
		int start;
		int finish;
		if (m[i].rm_so == -1) {
			break;
		}
		start = m[i].rm_so + (to_match - to_match);
		finish = m[i].rm_eo + (to_match - to_match);
		int _strlen = (finish - start);
		char *_str = to_match + start;
		_str[_strlen] = '\0';
		return _str;
	}
	return 0;
}

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
		printf("A configuration file must be given.\n");
		return 1;
	}

	if (access(config_path, F_OK) == -1) {
		printf("%s: no such file or directory.\n", config_path);
		return 1;
	}

	config_t *config = config_from_yaml(config_path);
	printf("Configuration parsed: %d accounts, %d min scan interval.\n",
	       config->accounts_size,
	       config->scan_interval);

	// g_app = g_create("it.davidepucci.PaperBoy");
	// g_run(g_app);

	// printf("user %s password %s server %s\n", user_address, user_password,
	// mail_server);

	// struct sockaddr_in server;
	// struct hostent *host;
	// int sock;

	// for (;;) {
	// 	sock = socket(AF_INET, SOCK_STREAM, 0);
	// 	if (sock == -1) {
	// 		printf("Unable to create socket: %d.", errno);
	// 		exit(EXIT_FAILURE);
	// 	}

	// 	server.sin_family = AF_INET;
	// 	server.sin_port = htons(143);
	// 	host = gethostbyname(account->hostname);
	// 	if (host == NULL) {
	// 		printf("Failed on gethostbyname: %s.", hstrerror(h_errno));
	// 		exit(EXIT_FAILURE);
	// 	}
	// 	memcpy(&server.sin_addr, host->h_addr_list[0], host->h_length);

	// 	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
	// 		printf("Failed on connecting to server socket.\n");
	// 		return -1;
	// 	}

	// 	char *message[512];
	// 	char *buffer[512];
	// 	for (;;) {
	// 		int packet_length = recv(sock, buffer, 512, 0);
	// 		buffer[packet_length] = '\0';
	// 		// printf("%s", buffer);
	// 		if (packet_length != 512) {
	// 			break;
	// 		}
	// 	}
	// 	// printf("\n");

	// 	sprintf(message, "1 LOGIN %s %s\r\n", account->address, account->password);
	// 	if (write(sock, message, strlen(message)) < 0) {
	// 		printf("Unable to send message.\n");
	// 		return -1;
	// 	}

	// 	for (;;) {
	// 		int packet_length = recv(sock, buffer, 512, 0);
	// 		buffer[packet_length] = '\0';
	// 		// printf("%s", buffer);
	// 		if (packet_length != 512) {
	// 			break;
	// 		}
	// 	}
	// 	// printf("\n");

	// 	sprintf(message, "1 STATUS INBOX (UNSEEN)\r\n");
	// 	if (write(sock, message, strlen(message)) < 0) {
	// 		printf("Unable to send message.\n");
	// 		return -1;
	// 	}

	// 	int packet_length = recv(sock, buffer, 512, 0);
	// 	buffer[packet_length] = '\0';

	// 	// printf("%s\n", buffer);

	// 	regex_t r;
	// 	const char *regex_text = ".*\\(UNSEEN ([[:digit:]]+)\\).*";
	// 	int status = regcomp(&r, regex_text, REG_EXTENDED | REG_NEWLINE);
	// 	if (status != 0) {
	// 		printf("Regex error compiling '%s'\n", regex_text);
	// 		return 1;
	// 	}

	// 	const int unseen = atoi(first_match(&r, buffer));

	// 	printf("Unseen: %d\n", unseen);

	// 	char *notify_title = (char *)calloc(25, sizeof(char *));
	// 	char *notify_body = (char *)calloc(150, sizeof(char *));
	// 	sprintf(notify_title, "Status for %s", account->address);
	// 	sprintf(notify_body, "Emails unread %d", unseen);
	// 	g_notify(g_app, notify_title, notify_body);

	// 	regfree(&r);
	// 	sleep(60);
	// }

	// close(sock);
	// g_shutdown(g_app);

	return 0;
}
