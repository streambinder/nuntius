#include <stdlib.h>
#include <string.h>

#include "account.h"

extern account_t *account_new()
{
	return (account_t *)malloc(sizeof(account_t));
}

extern account_t *account_parse(char *alias, char *address, char *password, char *hostname,
				char *proto, char *port, char *url)
{
	account_t *account = account_new();
	account->alias = strdup(alias);
	account->address = strdup(address);
	account->password = strdup(password);
	account->hostname = strdup(hostname);
	account->proto = strdup(proto);
	account->port = strdup(port);
	account->url = strdup(url);
	return account;
}
