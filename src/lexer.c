#include <BASM/lexer.h>

//#define DEBUG

static struct token *head;
static struct token *current_t;

static size_t pos;
static uint16_t line;
static uint16_t column;

static const char *str;
static FILE *f;

static char current;

static enum { M_FILE, M_STR } mode;

static inline char next_c()
{
        column++;
        if (mode == M_FILE) {
                char v = (char)fgetc(f);
                return current = v == EOF ? '\0' : v;
        } else {
                return current = *str++;
        }
}

static inline struct tok_location get_location()
{
        struct tok_location loc = {
                .column = column,
                .line = line
        };

        return loc;
}

static void add_token(struct token val)
{
        if (current_t == NULL) {
                current_t = head;
        } else {
                current_t->next = malloc(sizeof(val));

                if (current_t->next == NULL) {
                        return;
                }
                current_t = current_t->next;
        }

#ifdef DEBUG
        printf("\n----\nADDT\nTYPE: %d\n----\n", val.type);
#endif


        memcpy(current_t, &val, sizeof(val));
        current_t->next = NULL;
}

static void erase_token(struct token *t)
{
        if (t->type == TT_ID) {
                free(t->id_val);
                t->id_val = NULL;
        }
}


void delete_tokens(struct token *list)
{
        if (list == NULL) {
                return;
        }

        while (list) {
                struct token *tmp = list;

                list = list->next;

                erase_token(tmp);

                free(tmp);
        }
}

static inline int init_lexer()
{
        pos = 0;
        line = 1;
        column = 0;

        if (head) {
                delete_tokens(head);
        }

        head = malloc(sizeof(struct token));

        if (!head) {
                return BASM_ALLOC_ERR;
        }

        current = next_c();

        current_t = NULL;

        return 0;
}

static inline int clear_lexer()
{
        pos = 0;
        line = 0;
        column = 0;

        if (head) {
                delete_tokens(head);
        }

        head = NULL;
        current_t = NULL;

        str = NULL;
        f = NULL;

        return 0;
}

static void tokenize_op()
{
        struct tok_location loc = get_location();

        char op = current;
        next_c();

        enum token_type type;

        switch (op) {
                case '+':
                        type = TT_PLUS;
                        break;
                case '-':
                        type = TT_MINUS;
                        break;
                case '*':
                        type = TT_STAR;
                        break;
                case '/':
                        type = TT_SLASH;
                        break;
                case '=':
                        type = TT_ASSIGN;
                        break;
                default:
                        type = TT_UNKNOWN;
                        break;
        }

        struct token t = {
                .loc = loc,
                .type = type
        };

        add_token(t);
}

static void tokenize_word()
{
        struct char_buffer buff = create_char_buffer();

        struct tok_location loc = get_location();

        while (isalnum(current) || current == '_') {
                append_char(&buff, current);
                next_c();
        }

        char *word = buffer_to_str(&buff);

        free_char_buffer(&buff);

        struct token result = {
                .loc = loc,
                .next = NULL,
                .type = TT_ID
        };

        if (strcmp("let", word) == 0) {
                result.type = TT_LET;
        } else if (strcmp("print", word) == 0) {
                result.type = TT_PRINT;
        } else {
                result.id_val = word;
                goto end;
        }

        free(word);


end:
        add_token(result);
}

static void tokenize_num()
{
        struct tok_location loc = get_location();

        int num = 0;

        while (isdigit(current)) {
                num = num * 10 + (current - '0');
                next_c();
        }

        struct token t = {
                .loc = loc,
                .type = TT_NUM_LITERAL,
                .int_val = num
        };

        add_token(t);
}

static struct token *tokenize()
{
        while (current) {
                if (isalpha(current)) {
                        tokenize_word();
                } else if (current == '+' || current == '-' || 
                        current == '*' || current == '/' || current == '=') {
                        
                        tokenize_op();
                } else if (isdigit(current)) {
                        tokenize_num();
                } else if (current == '\n') {
                        line++;
                        next_c();
                        column = 1;
                }
                else if (current == ' ') {
                        next_c();
                } else {
                        struct token t = {
                                .type = TT_UNKNOWN
                        };
                        add_token(t);
                        next_c();
                }
        }

        struct token eof = {
                .loc.column = column,
                .loc.line = line,
                .type = TT_EOF
        };

        add_token(eof);

        struct token *result = head;

        current_t = NULL;
        head = NULL;

        clear_lexer();

        return result;
}

struct token *tokenize_file(FILE *_f)
{
        mode = M_FILE;
        f = _f;
        init_lexer();

        return tokenize();
}

struct token *tokenize_str(const char *_str)
{
        mode = M_STR;
        str = _str;
        init_lexer();

        return tokenize();
}
