#ifndef PAPERBOY_CONFIG_H
#define PAPERBOY_CONFIG_H

#include "account.h"

typedef struct {
	account_t **accounts;
	int accounts_size;
	int scan_interval;
} config_t;

typedef enum {
	CONFIG_ACCOUNTS,
} config_parents_e;

typedef enum {
	CONFIG_TYPE_STR,
	CONFIG_TYPE_INT,
} config_type_e;

config_t *config_from_yaml(char *filename);

#endif /* PAPERBOY_CONFIG_H */