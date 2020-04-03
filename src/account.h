#ifndef PAPERBOY_ACCOUNT_H
#define PAPERBOY_ACCOUNT_H

typedef struct {
	char *alias;
	char *address;
	char *password;
	char *hostname;
	char *proto;
	char *port;
	char *url;
} account_t;

account_t *account_parse(char *alias, char *address, char *password, char *hostname, char *proto,
			 char *port, char *url);

#endif /* PAPERBOY_ACCOUNT_H */