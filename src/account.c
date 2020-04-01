#include "account.h"

static account_t *account_new()
{
	return (struct account_t *)malloc(sizeof(account_t));
}

extern account_t *account_parse(char *address, char *password, char *hostname, char *url)
{
	account_t *account = account_new();
	account->address = strdup(address);
	account->password = strdup(password);
	account->hostname = strdup(hostname);
	account->url = strdup(url);
	return account;
}
