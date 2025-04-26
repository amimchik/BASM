#include <BASM/interpreter.h>

static struct dict vars;

static void init_interpreter()
{
        vars = create_dict();
}

static void free_interpreter()
{
        free_dict(&vars);
}

static int evaluate_node(struct ast_node *n);

static int evaluate_bin_node(struct ast_node *node)
{
        int l = evaluate_node(node->bin_v.left);
        int r = evaluate_node(node->bin_v.right);

        switch (node->type) {
                case NT_ADD:
                        return l + r;
                        break;
                case NT_MIN:
                        return l - r;
                        break;
                case NT_MUL:
                        return l * r;
                        break;
                case NT_DIV:
                        return l / r;
                        break;
                default:
                        return 0;
                        break;
        }
}

static int evaluate_varc_node(struct ast_node *node)
{
        if (!node->varc_v.id) {
                return 0;
        }
        int *v = get_at(&vars, node->varc_v.id);
        return v ? *v : 0;
}

static int evaluate_assign_node(struct ast_node *node)
{
        if (!node->assign_v.value) {
                return 0;
        }

        set_at(&vars, node->assign_v.id, evaluate_node(node->assign_v.value));
        return 0;
}

static int evaluate_let_node(struct ast_node *node)
{
        if (!(node->decl_v.value && node->decl_v.id)) {
                return 0;
        }

        set_at(&vars, node->decl_v.id, evaluate_node(node->decl_v.value));
        return 0;
}

static int evaluate_print_node(struct ast_node *node)
{
        if (!node->print_v.value) {
                return 0;
        }

        printf("%d\n", evaluate_node(node->print_v.value));
        return 0;
}

static int evaluate_list_node(struct ast_node *node)
{
        struct ast_node *nodes = node->list_v.nodes;
        while (nodes) {
                evaluate_node(nodes);
                nodes = nodes->next;
        }

        return 0;
}

static int evaluate_node(struct ast_node *node)
{
        if (!node) {
                return 0;
        }
        switch (node->type) {
                case NT_ADD:
                case NT_DIV:
                case NT_MUL:
                case NT_MIN:
                        return evaluate_bin_node(node);
                        break;
                case NT_ASSIGN:
                        return evaluate_assign_node(node);
                        break;
                case NT_LET:
                        return evaluate_let_node(node);
                        break;
                case NT_LIST:
                        return evaluate_list_node(node);
                        break;
                case NT_NUMLIT:
                        return node->numlit_v.val;
                        break;
                case NT_VARC:
                        return evaluate_varc_node(node);
                        break;
                case NT_PRINT:
                        return evaluate_print_node(node);
                        break;
                default:
                        return 0;
                        break;
        }
}

void interpret_ast(struct ast_node *node)
{
        init_interpreter();
        evaluate_node(node);
        free_interpreter();
}