#ifndef CRB_EXPRESSION_H
#define CRB_EXPRESSION_H

#include "crb_type.h"

enum {
	CRB_EXPRESSION_NONE,
	CRB_INT_EXPRESSION,
	CRB_DOUBLE_EXPRESSION,
	CRB_STRING_EXPRESSION,
	CRB_IDENTIFIER_EXPRESSION,
	CRB_BINARY_EXPRESSION,
	CRB_ASSIGN_EXPRESSION,
	CRB_MINUS_EXPRESSION,
};

enum {
	CRB_BINARY_OPERATOR_NONE,
	CRB_BINARY_OPERATOR_ADD,
	CRB_BINARY_OPERATOR_SUB,
	CRB_BINARY_OPERATOR_MUL,
	CRB_BINARY_OPERATOR_DIV,
	CRB_BINARY_OPERATOR_MOD,
	CRB_BINARY_OPERATOR_GT,
	CRB_BINARY_OPERATOR_GE,
	CRB_BINARY_OPERATOR_LT,
	CRB_BINARY_OPERATOR_LE,
	CRB_BINARY_OPERATOR_EQ,
	CRB_BINARY_OPERATOR_NE,
	CRB_BINARY_OPERATOR_LOGICAL_AND,
	CRB_BINARY_OPERATOR_LOGICAL_OR,
};

struct crb_expression;

struct crb_binary_expression {
	int binary_operator;
	const struct crb_expression *left, *right;
};

struct crb_assign_expression {
	char *variable;
	const struct crb_expression *exprand;
};

struct crb_minus_expression {
	const struct crb_expression *expression;
};

struct crb_expression {
	int type;
	union {
		int int_value;
		double double_value;
		char *identifier;
		struct crb_string string_value;
		struct crb_binary_expression binary_expression;
		struct crb_assign_expression assign_expression;
		struct crb_minus_expression minus_expression;
	} u;
};

#define crb_expression_type_is_valid(_t_) ((_t_) > CRB_EXPRESSION_NONE\
		&& (_t_) <= CRB_MINUS_EXPRESSION)
#define crb_binary_expression_operator_is_valid(_o_) (\
		(_o_) > CRB_BINARY_OPERATOR_NONE\
		&& (_o_) <= CRB_BINARY_OPERATOR_LOGICAL_OR)

extern struct crb_expression *crb_create_expression(int type, void *value);

#define crb_create_int_expression(_v_) ({\
	int v = (_v_);\
	crb_create_expression(CRB_INT_EXPRESSION, &v);\
})
#define crb_create_double_expression(_v_) ({\
	double v = (_v_);\
	crb_create_expression(CRB_DOUBLE_EXPRESSION, &v);\
})
#define crb_create_string_expression(_v_) ({\
	struct crb_string v = (_v_);\
	crb_create_expression(CRB_STRING_EXPRESSION, &v);\
})

extern struct crb_expression *crb_create_binary_expression(int opr,
		const struct crb_expression *left,
		const struct crb_expression *right);

extern struct crb_expression *crb_create_assign_expression(char *variable,
		const struct crb_expression *exprand);

extern struct crb_expression *crb_create_minus_expression(
		const struct crb_expression *exp);

#endif // CRB_EXPRESSION_H

