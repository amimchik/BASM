#include <BASM/basm.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#ifndef CHAR_BUFFER_H
#define CHAR_BUFFER_H

#define CHR_BUFF_NODE_LEN 20

struct char_buffer_node {
        char content[CHR_BUFF_NODE_LEN];
        size_t used;
        struct char_buffer_node *next;
};

struct char_buffer {
        struct char_buffer_node         *head;          /*First element ptr*/
        struct char_buffer_node         *current;       /*Current element ptr(for live building)*/
        size_t                          len;
};

struct char_buffer create_char_buffer();

int append_char(struct char_buffer *buff, char c);
int append_str(struct char_buffer *buff, const char *s);


void free_char_buffer(struct char_buffer *buff);


char *buffer_to_str(struct char_buffer *buff);


int buff_append_f(struct char_buffer *buff, const char *format, ...);



#endif /*CHAR_BUFFER_H*/