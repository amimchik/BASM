#ifndef PARSER_H
#define PARSER_H


#include <BASM/basm.h>
#include <BASM/lexer.h>


enum node_type {
        NT_ADD,
        NT_MIN,
        NT_MUL,
        NT_DIV,

        NT_ASSIGN,

        NT_PRINT,

        NT_VARC,

        NT_NUMLIT,

        NT_LIST,
        NT_LET,
};

struct ast_node {
        int type;

        struct tok_location loc;
        
        union {        
                struct {
                        struct ast_node *left;
                        struct ast_node *right;
                } bin_v;

                struct {
                        char *id;
                        struct ast_node *value;
                } assign_v;

                struct {
                        char *id;
                        struct ast_node *value;
                } decl_v;

                struct {
                        struct ast_node *value;
                } print_v;
                
                struct {
                        struct ast_node *nodes;
                } list_v;

                struct {
                        char *id;
                } varc_v;

                struct {
                        int val;
                } numlit_v;
        };

        struct ast_node *next;  /*For linked list*/
};

struct ast_node *parse(struct token *t);

void delete_ast(struct ast_node *n);


#endif /*PARSER_H*/