#include <stdlib.h>
#include <string.h>
#include <yaml.h>

#include "config.h"

static config_t *config_new()
{
	config_t *config = (config_t *)malloc(sizeof(config_t));
	config->accounts = NULL;
	config->accounts_size = 0;
	config->scan_interval = 0;
	return config;
}

static config_t *config_parse(unsigned char *buffer, size_t length)
{
	config_t *config = config_new();

	yaml_parser_t parser;
	yaml_token_t token;

	yaml_parser_initialize(&parser);
	yaml_parser_set_input_string(&parser, buffer, length);

	int is_value = 0;
	int token_parent = -1;
	int token_key_type;
	char *token_val;
	void **token_dst;

	account_t *account = NULL;

	do {
		yaml_parser_scan(&parser, &token);

		switch (token.type) {
		case YAML_KEY_TOKEN:
			is_value = 0;
			break;
		case YAML_VALUE_TOKEN:
			is_value = 1;
			break;
		case YAML_BLOCK_END_TOKEN:
			if (token_parent == CONFIG_ACCOUNTS && account != NULL) {
				config->accounts =
				    realloc(config->accounts,
					    (config->accounts_size + 1) * sizeof(account_t));
				config->accounts[config->accounts_size++] = account;
				account = NULL;
			}
			break;
		case YAML_BLOCK_ENTRY_TOKEN:
			if (token_parent == CONFIG_ACCOUNTS) {
				account = account_new();
			}
			break;
		case YAML_SCALAR_TOKEN:
			token_val = (char *)token.data.scalar.value;
			if (is_value == 0) {
				if (!strcmp(token_val, "accounts")) {
					token_parent = CONFIG_ACCOUNTS;
				} else if (!strcmp(token_val, "scan_interval")) {
					token_parent = -1;
					token_dst = (void *)&config->scan_interval;
					token_key_type = CONFIG_TYPE_INT;
				} else if (token_parent == CONFIG_ACCOUNTS &&
					   !strcmp(token_val, "alias")) {
					token_dst = (void *)&account->alias;
					token_key_type = CONFIG_TYPE_STR;
				} else if (token_parent == CONFIG_ACCOUNTS &&
					   !strcmp(token_val, "address")) {
					token_dst = (void *)&account->address;
					token_key_type = CONFIG_TYPE_STR;
				} else if (token_parent == CONFIG_ACCOUNTS &&
					   !strcmp(token_val, "password")) {
					token_dst = (void *)&account->password;
					token_key_type = CONFIG_TYPE_STR;
				} else if (token_parent == CONFIG_ACCOUNTS &&
					   !strcmp(token_val, "hostname")) {
					token_dst = (void *)&account->hostname;
					token_key_type = CONFIG_TYPE_STR;
				} else if (token_parent == CONFIG_ACCOUNTS &&
					   !strcmp(token_val, "proto")) {
					token_dst = (void *)&account->proto;
					token_key_type = CONFIG_TYPE_STR;
				} else if (token_parent == CONFIG_ACCOUNTS &&
					   !strcmp(token_val, "port")) {
					token_dst = (void *)&account->port;
					token_key_type = CONFIG_TYPE_STR;
				} else if (token_parent == CONFIG_ACCOUNTS &&
					   !strcmp(token_val, "url")) {
					token_dst = (void *)&account->url;
					token_key_type = CONFIG_TYPE_STR;
				} else {
					fprintf(stderr,
						"[config] unrecognized key: %s\n",
						token_val);
					return NULL;
				}
			} else {
				switch (token_key_type) {
				case CONFIG_TYPE_INT:
					*token_dst = (void *)strtol(token_val, (char **)NULL, 10);
					break;
				case CONFIG_TYPE_STR:
				default:
					*token_dst = strdup(token_val);
					break;
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
	unsigned char *buffer = NULL;
	long length = 0;
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