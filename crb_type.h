#ifndef CRB_TYPE_H
#define CRB_TYPE_H

#include "util/crb_stack.h"
#include <stdlib.h>

struct crb_string {
	CRB_STACK_HEADER;
};

extern int crb_string_init(struct crb_string *str, size_t capacity);
extern int crb_string_append_char(struct crb_string *str, char c);
extern size_t crb_string_append_chars(struct crb_string *str, char *chars);

struct crb_block {
	int type;
	struct crb_stack statements;
};

struct crb_function {
	struct crb_stack parameters;
	struct crb_block block;
};

enum {
	CRB_NULL_VALUE,
	CRB_BOOLEAN_VALUE,
	CRB_INT_VALUE,
	CRB_DOUBLE_VALUE,
	CRB_STRING_VALUE,
	CRB_FUNCTION_VALUE,
};

struct crb_value {
	int type;
	union {
		int boolean_value;
		int int_value;
		double float_value;
		struct crb_string string_value;
		struct crb_function function_value;
	} u;
};

extern void crb_value_print(struct crb_value v);

#define CRB_NULL ((struct crb_value){0})

#define crb_is_null(_v_) ((_v_).type == CRB_NULL_VALUE)

#define crb_is_boolean_value(_v_) ((_v_).type == CRB_BOOLEAN_VALUE)

#define crb_is_numberical_value(_v_) ((_v_).type == CRB_INT_VALUE\
		|| (_v_).type == CRB_DOUBLE_VALUE)

#define crb_is_string_value(_v_) ((_v_).type == CRB_STRING_VALUE)

#define crb_is_function_value(_v_) ((_v_).type == CRB_FUNCTION_VALUE) 

#endif //CRB_TYPE_H
