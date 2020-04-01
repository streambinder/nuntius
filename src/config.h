#ifndef CONFIG_H
#define CONFIG_H

#include "account.h"

typedef struct {
	account_t **accounts;
	int accounts_size;
	int scan_interval;
} config_t;

typedef enum {
	CONFIG_ACCOUNTS,
	CONFIG_SCAN_INTERVAL,
} config_field_e;

const static char *ACCOUNT_TUPLE_SEP = "|";

#endif /* CONFIG_H */