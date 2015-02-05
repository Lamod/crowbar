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
	CRB_NULL_VALUE,
	CRB_BOOLEAN_VALUE,
	CRB_INT_VALUE,
	CRB_DOUBLE_VALUE,
	CRB_STRING_VALUE,
};

struct crb_value {
	int type;
	union {
		int boolean_value;
		int int_value;
		double float_value;
		struct crb_string string_value;
	} u;
};

extern void crb_value_print(struct crb_value v);

#define CRB_NULL ((struct crb_value){0})

#define crb_is_boolean_value(_v_) ((_v_).type == CRB_BOOLEAN_VALUE)
#define crb_is_numberical_value(_v_) ((_v_).type == CRB_INT_VALUE\
		|| (_v_).type == CRB_DOUBLE_VALUE)

#endif //CRB_TYPE_H
