#include <BASM/basm.h>
#include <BASM/parser.h>
#include <BASM/dict.h>

#ifndef INTERPRETER_H
#define INTERPRETER_H


void interpret_ast(struct ast_node *node);


#endif /*INTERPRETER_H*/