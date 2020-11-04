#ifndef NUNTIUS_IMAP_H
#define NUNTIUS_IMAP_H

typedef struct {
	char memory[100 * 1024];
	size_t size;
} imap_memory_t;

int imap_unread(account_t *account);

#endif /* NUNTIUS_IMAP_H */