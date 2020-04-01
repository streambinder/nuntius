#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "imap.h"

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

extern int imap_unread(char *username, char *password, char *hostname)
{
	struct sockaddr_in server;
	struct hostent *host;
	int sock;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		printf("[imap] unable to create socket: %d.", errno);
		return -1;
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(143);
	host = gethostbyname(hostname);
	if (host == NULL) {
		printf("[imap] unable to resolve %s: %s.", hostname, hstrerror(h_errno));
		return -1;
	}
	memcpy(&server.sin_addr, host->h_addr_list[0], host->h_length);

	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
		printf("[imap] unable to connect to server %s socket.\n", hostname);
		return -1;
	}

	char *message[512];
	char *buffer[512];
	for (;;) {
		int packet_length = recv(sock, buffer, 512, 0);
		buffer[packet_length] = '\0';
		if (packet_length != 512) {
			break;
		}
	}

	sprintf(message, "1 LOGIN %s %s\r\n", username, password);
	if (write(sock, message, strlen(message)) < 0) {
		printf("[imap] unable to login to %s with user %s.\n", hostname, username);
		return -1;
	}

	for (;;) {
		int packet_length = recv(sock, buffer, 512, 0);
		buffer[packet_length] = '\0';
		if (packet_length != 512) {
			break;
		}
	}

	sprintf(message, "1 STATUS INBOX (UNSEEN)\r\n");
	if (write(sock, message, strlen(message)) < 0) {
		printf("[imap] unable to get unread emails.\n");
		return -1;
	}

	int packet_length = recv(sock, buffer, 512, 0);
	buffer[packet_length] = '\0';

	regex_t r;
	const char *regex_text = ".*\\(UNSEEN ([[:digit:]]+)\\).*";
	int status = regcomp(&r, regex_text, REG_EXTENDED | REG_NEWLINE);
	if (status != 0) {
		printf("[imap] unable to compile regex '%s'\n", regex_text);
		return 1;
	}

	const int unreads = atoi(first_match(&r, buffer));
	regfree(&r);
	close(sock);

	return unreads;
}