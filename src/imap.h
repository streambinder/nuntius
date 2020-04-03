#ifndef IMAP_H
#define IMAP_H

typedef struct {
	char memory[100 * 1024];
	size_t size;
} imap_memory_t;

imap_unread(account_t *account);

#endif /* IMAP_H */