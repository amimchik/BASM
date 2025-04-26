#include <BASM/dict.h>


struct dict create_dict()
{
        struct dict d = {
                .head = NULL
        };
        return d;
}

static struct dict_node *create_node(const char *key, int value)
{
        struct dict_node *n = malloc(sizeof(struct dict_node));

        if (!n) {
                goto alloc_err;
        }

        n->key = malloc(strlen(key) + 1);

        if (!n->key) {
                goto alloc_err;
        }

        strcpy(n->key, key);

        n->value = value;

        n->next = NULL;

        return n;

alloc_err:
        if (!n) {
                goto alloc_err_end;
        }

        if (n->key) {
                free(n->key);
        }

        free(n);

        n = NULL;

alloc_err_end:
        return NULL;
}

int set_at(struct dict *d, const char *key, int value)
{
        if (!key) {
                return 0;
        }

        struct dict_node *n = d->head;

        if (!n) {
                d->head = n = create_node(key, value);
                if (!n) {
                        goto alloc_err;
                }
                goto ok;
        }

        while (n) {
                if (!strcmp(key, n->key)) {
                        n->value = value;
                        goto ok;
                }
                if (n->next) {
                        n = n->next;
                } else {
                        break;
                }
        }

        n->next = create_node(key, value);

        if (!n->next) {
                goto alloc_err;
        }

ok:
        return 0;

alloc_err:
        return BASM_ALLOC_ERR;
}

int *get_at(struct dict *d, const char *key)
{
        struct dict_node *n = d->head;

        while (n) {
                if (!strcmp(key, n->key)) {
                        return &n->value;
                }
                n = n->next;
        }

        return NULL;
}

static void free_node(struct dict_node *n)
{
        if (!n) {
                return;
        }

        free(n->key);

        free(n);
}


void free_dict(struct dict *d)
{
        struct dict_node *node = d->head;

        while (node) {
                struct dict_node *temp = node;

                node = node->next;

                free_node(temp);
        }
}