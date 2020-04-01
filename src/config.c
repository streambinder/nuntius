#include <stdlib.h>
#include <string.h>
#include <yaml.h>

#include "config.h"

static account_t *config_parse_account_tuple(char *tuple)
{
	char *address = strdup(strtok(tuple, ACCOUNT_TUPLE_SEP));
	char *password = strdup(strtok(NULL, ACCOUNT_TUPLE_SEP));
	char *hostname = strdup(strtok(NULL, ACCOUNT_TUPLE_SEP));
	char *url = strdup(strtok(NULL, ACCOUNT_TUPLE_SEP));
	return account_parse(address, password, hostname, url);
}

static config_t *config_new()
{
	config_t *config = (struct config_t *)malloc(sizeof(config_t));
	config->accounts = NULL;
	config->accounts_size = 0;
	config->scan_interval = 0;
	return config;
}

static config_t *config_parse(char *buffer, size_t length)
{
	config_t *config = config_new();

	yaml_parser_t parser;
	yaml_token_t token;

	yaml_parser_initialize(&parser);
	yaml_parser_set_input_string(&parser, buffer, length);

	int is_value = 0;
	char *token_val;
	char token_key;

	do {
		yaml_parser_scan(&parser, &token);

		switch (token.type) {
		case YAML_KEY_TOKEN:
			is_value = 0;
			break;
		case YAML_VALUE_TOKEN:
			is_value = 1;
			break;
		case YAML_SCALAR_TOKEN:
			token_val = token.data.scalar.value;
			if (is_value == 0) {
				if (!strcmp(token_val, "scan_interval")) {
					token_key = CONFIG_SCAN_INTERVAL;
				} else if (!strcmp(token_val, "account")) {
					token_key = CONFIG_ACCOUNTS;
				} else {
					printf("[config] unrecognized key: %s\n", token_val);
				}
			} else {
				if (token_key == CONFIG_SCAN_INTERVAL) {
					config->scan_interval =
					    strtol(token_val, (char **)NULL, 10);
				} else if (token_key == CONFIG_ACCOUNTS) {
					account_t *account = config_parse_account_tuple(token_val);
					config->accounts = realloc(config->accounts,
								   (config->accounts_size + 1) *
								       sizeof(account_t));
					config->accounts[config->accounts_size++] = account;
				}
			}
			break;
		default:
			break;
		}

		if (token.type != YAML_STREAM_END_TOKEN) {
			yaml_token_delete(&token);
		}
	} while (token.type != YAML_STREAM_END_TOKEN);

	yaml_token_delete(&token);
	yaml_parser_delete(&parser);

	return config;
}

extern config_t *config_from_yaml(char *filename)
{
	char *buffer = 0;
	long length;
	FILE *file = fopen(filename, "rb");

	if (file) {
		fseek(file, 0, SEEK_END);
		length = ftell(file);
		fseek(file, 0, SEEK_SET);
		buffer = malloc(length);
		if (buffer) {
			fread(buffer, 1, length, file);
		}
		fclose(file);
	}

	return config_parse(buffer, length);
}