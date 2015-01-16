#ifndef CRB_STRING_H
#define CRB_STRING_H

#include <stdlib.h>

struct crb_string {
	size_t capacity, length;
	char *data;
};

extern int crb_string_init(struct crb_string *str, size_t capacity);
extern int crb_string_append_char(struct crb_string *str, char c);
extern size_t crb_string_append_chars(struct crb_string *str, char *chars);

#endif //CRB_STRING_H
