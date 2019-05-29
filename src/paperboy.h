#ifndef PAPERBOY_H
#define PAPERBOY_H

typedef struct
{
    char address[320];
    char password[500];
    char hostname[255];
    char url[500];
} account_t;

#endif /* PAPERBOY_H */