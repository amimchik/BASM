#include <BASM/basm.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#ifndef DICT_H
#define DICT_H

struct dict_node {
        const char *key;
        int value;
        struct dict_node *next;
};

struct dict {
        struct dict_node *head;
};

struct dict create_dict();

int set_at(struct dict *d, const char *key, int value);

int *get_at(struct dict *d, const char *key);

void free_dict(struct dict *d);


#endif /*DICT_H*/