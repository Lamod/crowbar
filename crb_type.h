#ifndef CRB_TYPE_H
#define CRB_TYPE_H

#include <stdlib.h>

struct crb_string {
	size_t capacity, length;
	char *data;
};

extern int crb_string_init(struct crb_string *str, size_t capacity);
extern int crb_string_append_char(struct crb_string *str, char c);
extern size_t crb_string_append_chars(struct crb_string *str, char *chars);

enum {
	CRB_INT_VALUE,
	CRB_DOUBLE_VALUE,
	CRB_STRING_VALUE,
};

struct crb_value {
	int type;
	union {
		int int_value;
		double float_value;
		struct crb_string string_value;
	} u;
};

#endif //CRB_TYPE_H
