#include <BASM/codegen.h>


static const char program_start[] = 
        "section .data\n"
        "buff db '0'\n"
        "section .text\n"
        "global _start\n"
        "_start:\n"
        "call program\n"
        "mov rax, 60\n"
        "xor rdi, rdi\n"
        "syscall\n"
        "print:\n"
        "push rcx\n"
	"call printnum\n"
	"mov sil, 10\n"
	"mov [buff], sil\n"
	"call printc\n"
        "pop rcx\n"
	"ret\n"
        "printnum:\n"
        "push rax\n"
        "push rbx\n"
        "push rcx\n"
        "push rdx\n"
        "cmp rax, 0\n"
        "jge .check_zero\n"
        "mov sil, '-'\n"
        "mov [buff], sil\n"
        "call printc\n"
        "neg rax\n"
        ".check_zero:\n"
        "mov rbx, 10\n"
        "xor rdx, rdx\n"
        "div rbx\n"
        "cmp rax, 0\n"
        "je .print_digit\n"
        "push rdx\n"
        "call printnum\n"
        "pop rdx\n"
        ".print_digit:\n"
        "add dl, '0'\n"
        "mov [buff], dl\n"
        "call printc\n"
        "pop rdx\n"
        "pop rcx\n"
        "pop rbx\n"
        "pop rax\n"
        "ret\n"
        "printc:\n"
        "push rdi\n"
        "push rsi\n"
        "push rdx\n"
        "push rax\n"
        "mov rdi, 1\n"
        "mov rsi, buff\n"
        "mov rdx, 1\n"
        "mov rax, 1\n"
        "syscall\n"
        "pop rax\n"
        "pop rdx\n"
        "pop rsi\n"
        "pop rdi\n"
        "ret\n"
        "program:\n";


static struct char_buffer buff;
static struct dict variables;
static int variables_count;
static int local_shift;


static void gen_node(struct ast_node *n);

static void new_variable(char *n)
{
        if (get_at(&variables, n))
        {
                return;
        }
        set_at(&variables, n, variables_count++);
}

static void append_line(const char *s)
{
        append_str(&buff, s);
        append_char(&buff, '\n');
}

static void append(const char *s)
{
        append_str(&buff, s);
}

static int get_offset(const char *s)
{
        int *p_offset = get_at(&variables, s);
        if (!p_offset) {
                return -1;
        }
        return 4 * (variables_count - *p_offset - 1) + local_shift;
}


static void gen_bin_node(struct ast_node *n)
{
        if (!n) {
                return;
        }

        append_line("sub rsp, 4");
        append_line("mov dword [rsp], ebx");

        local_shift += 4;

        gen_node(n->bin_v.right);

        append_line("mov ebx, eax");

        gen_node(n->bin_v.left);

        switch (n->type) {
                case NT_ADD:
                        append_line("add eax, ebx");
                        break;
                case NT_MIN:
                        append_line("sub eax, ebx");
                        break;
                case NT_DIV:
                        append_line("mov edx, 0\n"
                                "idiv ebx");
                        break;
                case NT_MUL:
                        append_line("imul eax, ebx");
                        break;
                default:
                        append_line("mov eax, ebx");
                        break;
        }

        append_line("mov ebx, [rsp]");
        append_line("add rsp, 4");
        local_shift -= 4;
}

static void gen_let_node(struct ast_node  *n)
{
        if (!n) {
                return;
        }

        gen_node(n->decl_v.value);

        new_variable(n->decl_v.id);

        append_line("sub rsp, 4");
        append_line("mov dword [rsp], eax");
}

static void gen_assign_node(struct ast_node *n)
{
        if (!n) {
                return;
        }

        gen_node(n->assign_v.value);

        buff_append_f(&buff, "mov [rsp + %d], eax\n", get_offset(n->assign_v.id));
}

static void gen_varc_node(struct ast_node *n)
{
        if (!n) {
                return;
        }

        buff_append_f(&buff, "mov eax, [rsp + %d]\n", get_offset(n->varc_v.id));
}

static void gen_list_node(struct ast_node *n)
{
        if (!n) {
                return;
        }

        struct ast_node *nodes = n->list_v.nodes;

        while (nodes) {
                gen_node(nodes);
                nodes = nodes->next;
        }
}

static void gen_node(struct ast_node *n)
{
        if (!n) {
                return;
        }
#ifdef DEBUG
        buff_append_f(&buff, ";BLOCK START condition: lshift: %d varc: %d\n", local_shift, variables_count);
#endif /*DEBUG*/
        switch (n->type) {
                case NT_ADD:
                case NT_MIN:
                case NT_DIV:
                case NT_MUL:
                        gen_bin_node(n);
                        break;
                case NT_NUMLIT:
                        buff_append_f(&buff, "mov eax, %d\n", n->numlit_v.val);
                        break;
                case NT_PRINT:
                        gen_node(n->print_v.value);
                        append_line("mov rcx, rsp");
                        append_line("and rsp, 0xFFFFFFFFFFFFFFF0");
                        append_line("call print");
                        append_line("mov rsp, rcx");
                        break;
                case NT_LET:
                        gen_let_node(n);
                        break;
                case NT_ASSIGN:
                        gen_assign_node(n);
                        break;
                case NT_VARC:
                        gen_varc_node(n);
                        break;
                case NT_LIST:
                        gen_list_node(n);
                        break;
        }
#ifdef DEBUG
        buff_append_f(&buff, ";BLOCK END condition: lshift: %d varc: %d\n", local_shift, variables_count);
#endif /*DEBUG*/
}

void init_codegen()
{
        buff = create_char_buffer();
        variables = create_dict();
        variables_count = 0;
        local_shift = 0;
}

void delete_codegen()
{
        free_char_buffer(&buff);
        free_dict(&variables);
        variables_count = 0;
        local_shift = 0;
}


char *gencode(struct ast_node *n)
{
        init_codegen();

        append(program_start);

        gen_node(n);

        buff_append_f(&buff, "add rsp, %d\n", variables_count * 4);

        append_line("ret");

        char *result = buffer_to_str(&buff);

        delete_codegen();

        return result;
}