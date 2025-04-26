#ifndef LEXER_H
#define LEXER_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <memory.h>

#include <BASM/basm.h>
#include <BASM/сhar_buffer.h>

/*
 *   Process stream of chars and convert to tokens stream
 */

enum token_type {
	TT_ID,

	TT_PRINT,

	TT_LET,

	TT_PLUS,
	TT_MINUS,
	TT_STAR,
	TT_SLASH,

	TT_ASSIGN,

	TT_NUM_LITERAL,

	TT_EOF,

	TT_UNKNOWN
};

struct tok_location {
	uint16_t 	line;
	uint16_t 	column;
};

struct token {
	int type;

	struct tok_location loc;

	union {
		char* 	id_val;
		int 	int_val;
	};

	struct token* 	next;
};

struct token *tokenize_str(const char *str);
struct token *tokenize_file(FILE *f);

void delete_tokens(struct token *list);

#endif /*LEXER_H*/