#include <BASM/сhar_buffer.h>


static inline struct char_buffer_node *extend_node(struct char_buffer_node *node) 
{
        node->next = malloc(sizeof(struct char_buffer_node));

        if (!node->next) {
                return NULL;
        }

        node->next->next = NULL;

        node->next->used = 0;

        return node->next;
}

static inline struct char_buffer_node *create_node(void)
{
        struct char_buffer_node *node = malloc(sizeof(struct char_buffer_node));

        if (!node) {
                return NULL;
        }

        node->next = NULL;
        node->used = 0;


        return node;
}

int buff_append_f(struct char_buffer *buff, const char *format, ...) {
        va_list args;
        va_start(args, format);

        char temp_buff[1024];
        
        int len = vsnprintf(temp_buff, sizeof(temp_buff), format, args);
        
        va_end(args);
        
        if (len < 0) {
            return -1;
        }
    
        return append_str(buff, temp_buff);
    }


struct char_buffer create_char_buffer()
{
        struct char_buffer buff = {
                .current = NULL,
                .head    = NULL,
                .len     = 0
        };

        buff.current = create_node();

        buff.head = buff.current;

        return buff;
}


int append_char(struct char_buffer *buff, char c)
{
        if (!buff) {
                return BASM_OBJ_UNDEFINED;
        }

        if (!buff->current) {
                buff->current = create_node();

                if (!buff->current) {
                        return BASM_ALLOC_ERR;
                }

                buff->head = buff->current;
        }

        if (buff->current->used == CHR_BUFF_NODE_LEN) {
                buff->current = extend_node(buff->current);
                if (!buff->current) {
                        return BASM_ALLOC_ERR;
                }
        }

        buff->current->content[buff->current->used++] = c;
        buff->len++;

        return 0;
}


int append_str(struct char_buffer *buff, const char *s)
{
        if (!buff) {
                return BASM_OBJ_UNDEFINED;
        }

        while (*s) {
                if (append_char(buff, *s++) != 0) {
                        return BASM_ALLOC_ERR;
                }
        }

        return 0;
}


void free_char_buffer(struct char_buffer *buff)
{
        if (!buff || !buff->head) {
                return;
        }  

        struct char_buffer_node *current = buff->head;
        struct char_buffer_node *temp;

        while (current) {
                temp = current;

                current = current->next;

                free(temp);
        }
}

char *buffer_to_str(struct char_buffer *buff)
{
        if (!buff) {
                return NULL;
        }

        char *s = malloc(buff->len + 1);
        s[buff->len] = '\0';

        size_t i = 0;

        struct char_buffer_node *current = buff->head;

        while (current) {
                for (size_t z = 0; z < current->used; z++, i++) {
                        s[i] = current->content[z];
                }
                current = current->next;
        }

        return s;
}