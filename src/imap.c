#include <curl/curl.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "account.h"
#include "imap.h"

static char *first_match(regex_t *r, char *to_match)
{
	/* "N_matches" is the maximum number of matches allowed. */
	const int n_matches = 10;
	/* "M" contains the matches found. */
	regmatch_t m[n_matches];
	int i = 0;

	int nomatch = regexec(r, to_match, n_matches, m, 0);
	if (nomatch) {
		return NULL;
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
	return NULL;
}

static size_t imap_memory_callback(void *content, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	imap_memory_t *mem = (imap_memory_t *)userp;
	memcpy(&(mem->memory[mem->size]), content, realsize);
	mem->size += realsize;
	mem->memory[mem->size + realsize] = 0;
	return realsize;
}

extern int imap_unread(account_t *account)
{
	CURL *curl;
	CURLcode res;
	imap_memory_t chunk;
	imap_memory_t header;

	const char *query = "INBOX?UNSEEN";
	char *url =
	    malloc(sizeof(char *) * (strlen(account->proto) + 3 + strlen(account->hostname) + 1 +
				     strlen(account->port) + 1 + strlen(query)));
	sprintf(url, "%s://%s:%s/%s", account->proto, account->hostname, account->port, query);
	printf("[imap] url: %s\n", url);

	char *userpwd =
	    malloc(sizeof(char *) * (strlen(account->address) + 1 + strlen(account->password)));
	sprintf(userpwd, "%s:%s", account->address, account->password);

	chunk.size = 0;
	header.size = 0;

	curl = curl_easy_init();
	if (!curl) {
		fprintf(stderr, "[imap] unable to initialize curl\n");
		return -1;
	}

	curl_easy_setopt(curl, CURLOPT_USERPWD, userpwd);
	curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, imap_memory_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void *)&header);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	curl_easy_setopt(curl, CURLOPT_URL, url);

	res = curl_easy_perform(curl);
	if ((res != CURLE_OK) && (res != CURLE_REMOTE_FILE_NOT_FOUND)) {
		fprintf(stderr, "[imap] unable to query: %s\n", curl_easy_strerror(res));
		return -1;
	}

	printf("[imap] %lu bytes retrieved\n", (long)chunk.size);
	if (*chunk.memory != 0) {
		// printf("[imap] unseen response: \"%s\"\n", chunk.memory);
		chunk.size = 0;
		header.size = 0;
	}

	regex_t regex;
	const char *regex_text = ".*SEARCH ([[:digit:]]+).*";
	int status = regcomp(&regex, regex_text, REG_EXTENDED | REG_NEWLINE);
	if (status != 0) {
		fprintf(stderr, "[imap] unable to compile regex: %s\n", regex_text);
		return -1;
	}

	char *match = first_match(&regex, header.memory);
	if (match == NULL) {
		fprintf(stderr, "[imap] no count match in IMAP response\n");
		return 0;
	}

	const int unreads = strtol(match, (char **)NULL, 10);
	regfree(&regex);
	return unreads;
}