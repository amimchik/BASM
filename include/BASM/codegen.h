#include <BASM/basm.h>
#include <BASM/parser.h>
#include <BASM/сhar_buffer.h>
#include <BASM/dict.h>

#ifndef CODEGEN_H
#define CODEGEN_H

char *gencode(struct ast_node *n);


#endif /*CODEGEN_H*/