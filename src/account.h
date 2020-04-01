#ifndef ACCOUNT_H
#define ACCOUNT_H

typedef struct {
	char *address;
	char *password;
	char *hostname;
	char *url;
} account_t;

#endif /* ACCOUNT_H */