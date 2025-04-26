#include <BASM/parser.h>

struct token *tokens_head;
struct token *current_token;

static void init_parser(struct token *head)
{
        tokens_head = head;
        current_token = tokens_head;
}

static struct token *get_current()
{
        return current_token;
}

static uint8_t advance()
{
        if (current_token->next) {
                current_token = current_token->next;
                return 1;
        }
        return 0;
}

static uint8_t match(int token_type)
{
        if (get_current()->type == token_type) {
                advance();
                return 1;
        }
        return 0;
}

static struct ast_node *make_bin(struct ast_node *l, int type, struct ast_node *r)
{
        struct ast_node *node = malloc(sizeof(struct ast_node));

        if (!node) {
                return NULL;
        }

        node->type = type;
        node->bin_v.left = l;
        node->bin_v.right = r;
        node->next = NULL;

        return node;
}

static struct ast_node *make_numlit(int v)
{
        struct ast_node *node = malloc(sizeof(struct ast_node));

        if (!node) {
                return NULL;
        }

        node->type = NT_NUMLIT;
        node->numlit_v.val = v;
        node->next = NULL;

        return node;
}

static struct ast_node *make_varc(char *v)
{
        struct ast_node *node = malloc(sizeof(struct ast_node));

        if (!node) {
                return NULL;
        }

        node->varc_v.id = malloc(strlen(v) + 1);

        node->type = NT_VARC;

        strcpy(node->varc_v.id, v);

        node->next = NULL;

        return node;
}

static struct ast_node *parse_primary()
{
        struct token *n = get_current();

        if (match(TT_NUM_LITERAL)) {
                return make_numlit(n->int_val);
        }
        if (match(TT_ID)) {
                return make_varc(n->id_val);
        }

        return NULL;
}

static struct ast_node *parse_multiplicative()
{
        struct ast_node *left = parse_primary();

        int op = get_current()->type;

        while (match(TT_STAR) || match(TT_SLASH)) {
                struct ast_node *right = parse_primary();

                left = make_bin(left, op == TT_STAR ? NT_MUL : NT_DIV, right);

                op = get_current()->type;
        }

        return left;
}

static struct ast_node *parse_additive()
{
        struct ast_node *left = parse_multiplicative();

        int op = get_current()->type;

        while (match(TT_PLUS) || match(TT_MINUS)) {
                struct ast_node *right = parse_multiplicative();

                left = make_bin(left, op == TT_PLUS ? NT_ADD : NT_MIN, right);

                op = get_current()->type;
        }

        return left;
}

static struct ast_node *parse_expr()
{
        return parse_additive();
}

static struct ast_node *parse_stmt()
{
        struct ast_node *node = malloc(sizeof(struct ast_node));
        node->next = NULL;

        struct token *t = get_current();


        if (match(TT_PRINT)) {
                node->type = NT_PRINT;
                node->print_v.value = parse_expr();
                return node;
        }
        if (match(TT_LET)) {
                node->type = NT_LET;

                node->decl_v.id = malloc(strlen(get_current()->id_val) + 1);
                strcpy(node->decl_v.id, get_current()->id_val);

                advance();
                if (!match(TT_ASSIGN)) {
                        return NULL;
                }
                node->decl_v.value = parse_expr();
                return node;
        }
        if (match(TT_ID)) {
                node->type = NT_ASSIGN;

                node->assign_v.id = malloc(strlen(t->id_val) + 1);
                strcpy(node->assign_v.id, t->id_val);

                if (!match(TT_ASSIGN)) {
                        return NULL;
                }
                node->assign_v.value = parse_expr();
                return node;
        }

        return node;
}

static struct ast_node *parse_list()
{
        struct ast_node *nodes = malloc(sizeof(struct ast_node));

        nodes->type = NT_LIST;
        nodes->list_v.nodes = NULL;
        nodes->next = NULL;

        struct ast_node *current = NULL;

        while (get_current()->type != TT_EOF) {
                struct ast_node *parsed = parse_stmt();

                if (!parsed) {
                        return NULL;
                }

                if (!current) {
                        current = parsed;
                        nodes->list_v.nodes = current;
                } else {
                        current->next = parsed;
                        current = current->next;
                }
                current->next = NULL;
        }

        return nodes;
}

struct ast_node *parse(struct token *t)
{
        init_parser(t);
        
        return parse_list();
}



static void delete_binary(struct ast_node *n)
{
        if (!n) {
                return;
        }

        delete_ast(n->bin_v.left);
        delete_ast(n->bin_v.right);

        free(n);
}

static void delete_let(struct ast_node *n)
{
        if (!n) {
                return;
        }

        free(n->decl_v.id);

        delete_ast(n->decl_v.value);

        free(n);
}

static void delete_assign(struct ast_node *n)
{
        if (!n) {
                return;
        }

        free(n->assign_v.id);

        delete_ast(n->assign_v.value);

        free(n);
}

static void delete_list(struct ast_node *n)
{
        if (!n) {
                return;
        }
        delete_ast(n->list_v.nodes);

        free(n);
}


/*Func that recursivly deletes ast*/
void delete_ast(struct ast_node *node)
{
        while (node) {
                struct ast_node *next = node->next;

                switch (node->type) {
                        case NT_ADD:
                        case NT_MIN:
                        case NT_MUL:
                        case NT_DIV:
                                delete_binary(node);
                                break;
                        case NT_LET:
                                delete_let(node);
                                break;
                        case NT_ASSIGN:
                                delete_assign(node);
                                break;
                        case NT_NUMLIT:
                                free(node);
                                break;
                        case NT_PRINT:
                                delete_ast(node->print_v.value);
                                free(node);
                                break;
                        case NT_VARC:
                                free(node->varc_v.id);
                                free(node);
                                break;
                        case NT_LIST:
                                delete_list(node);
                                break;
                        default:
                                free(node);
                                break;
                }

                node = next;
        }
}