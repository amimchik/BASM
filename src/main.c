#include <stdio.h>
#include <BASM/lexer.h>
#include <BASM/parser.h>
#include <BASM/interpreter.h>
#include <BASM/codegen.h>

void print_tokens(struct token *t)
{
        while (t) {
                if (t->type == TT_ID) {
                        printf("ID: %s;  \tline: %d;\tcolumn: %d\n", t->id_val, t->loc.line, t->loc.column);

                } else if (t->type == TT_ASSIGN) {
                        printf("ASSIGN;\t\tline: %d;\tcolumn: %d\n", t->loc.line, t->loc.column);

                } else if (t->type == TT_PLUS) {
                        printf("PLUS;\t\tline: %d;\tcolumn: %d\n", t->loc.line, t->loc.column);

                } else if (t->type == TT_MINUS) {
                        printf("MINUS;\t\tline: %d;\tcolumn: %d\n", t->loc.line, t->loc.column);

                } else if (t->type == TT_STAR) {
                        printf("STAR;\t\tline: %d;\tcolumn: %d\n", t->loc.line, t->loc.column);

                } else if (t->type == TT_SLASH) {
                        printf("SLASH;\t\tline: %d;\tcolumn: %d\n", t->loc.line, t->loc.column);

                } else if (t->type == TT_EOF) {
                        printf("EOF;\t\tline: %d;\tcolumn: %d\n", t->loc.line, t->loc.column); 

                } else if (t->type == TT_NUM_LITERAL) {
                        printf("NUM: %d; \tline: %d;\tcolumn: %d\n", t->int_val, t->loc.line, t->loc.column);

                } else if (t->type == TT_PRINT) {
                        printf("PRINT;\t\tline: %d;\tcolumn: %d\n", t->loc.line, t->loc.column); 

                } else if (t->type == TT_LET) {
                        printf("LET;\t\tline: %d;\tcolumn: %d\n", t->loc.line, t->loc.column);  

                } else {
                        printf("Unknown; \tline: %d;\tcolumn: %d\n", t->loc.line, t->loc.column);

                }
                t = t->next;
        }
}

void print_ast(struct ast_node *node)
{
        if (!node) {
                return;
        }

        switch (node->type) {
                case NT_ADD:
                        printf("(+ ");
                        print_ast(node->bin_v.left);
                        printf(" ");
                        print_ast(node->bin_v.right);
                        printf(")");
                        break;
                case NT_MIN:
                        printf("(- ");
                        print_ast(node->bin_v.left);
                        printf(" ");
                        print_ast(node->bin_v.right);
                        printf(")");
                        break;
                case NT_MUL:
                        printf("(* ");
                        print_ast(node->bin_v.left);
                        printf(" ");
                        print_ast(node->bin_v.right);
                        printf(")");
                        break;
                case NT_DIV:
                        printf("(/ ");
                        print_ast(node->bin_v.left);
                        printf(" ");
                        print_ast(node->bin_v.right);
                        printf(")");
                        break;
                case NT_LET:
                        printf("let (= %s ", node->decl_v.id);
                        print_ast(node->decl_v.value);
                        printf(")");
                        break;
                case NT_ASSIGN:
                        printf("%s = ", node->assign_v.id);
                        print_ast(node->assign_v.value);
                        break;
                case NT_NUMLIT:
                        printf("%d", node->numlit_v.val);
                        break;
                case NT_VARC:
                        printf("%s", node->varc_v.id);
                        break;
                case NT_PRINT:
                        printf("print(");
                        print_ast(node->print_v.value);
                        printf(")");
                        break;
                case NT_LIST: {
                        struct ast_node *cur = node->list_v.nodes;
                        while (cur) {
                                print_ast(cur);
                                printf("\n");
                                cur = cur->next;
                        }
                        break;
                }
                default:
                        printf("unknown");
        }
}

int main()
{
        FILE *f = fopen("/home/mark/Projects/CLang/BASM/test.basm", "r");

#ifdef DEBUG
        printf("\nSource:\n\n");

        int ch;
        while ((ch = fgetc(f)) != EOF) {
            putchar(ch);
        }

        rewind(f);

        printf("\n\nTokens:\n\n");
#endif /*DEBUG*/


        struct token *t = tokenize_file(f);

#ifdef DEBUG
        print_tokens(t);
        printf("Closing file...");
#endif /*DEBUG*/
        fclose(f);

        struct ast_node *node = parse(t);

#ifdef DEBUG
        printf("\nDeleting tokens...\n");
#endif /*DEBUG*/
        delete_tokens(t);

#ifdef DEBUG
        printf("\n\nAST:\n\n");

        print_ast(node);

        printf("\n\nInterpritation:\n\n");

        interpret_ast(node);

#endif /*DEBUG*/

        char *code = gencode(node);

#ifdef DEBUG

        printf("\n\nASM:\n\n");

        printf("%s", code);

        printf("Deleting ast...\n");
#endif /*DEBUG*/
        delete_ast(node);

#ifdef DEBUG
        printf("Writing asm code to file...\n");
#endif /*DEBUG*/

        f = fopen("output.asm", "w");

        fprintf(f, "%s", code);
#ifdef DEBUG

        printf("Deleting asm code...\n");
#endif /*DEBUG*/
        free(code);

        return 0;
}
