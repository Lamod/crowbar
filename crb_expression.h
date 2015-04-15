#ifndef CRB_EXPRESSION_H
#define CRB_EXPRESSION_H

#include "crb_type.h"

enum {
	CRB_EXPRESSION_NONE,
	CRB_BOOLEAN_EXPRESSION,
	CRB_INT_EXPRESSION,
	CRB_DOUBLE_EXPRESSION,
	CRB_STRING_EXPRESSION,
	CRB_FUNCTION_EXPRESSION,
	CRB_IDENTIFIER_EXPRESSION,
	CRB_FUNCTION_CALL_EXPRESSION,
	CRB_BINARY_EXPRESSION,
	CRB_ASSIGN_EXPRESSION,
	CRB_UNARY_EXPRESSION,
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

enum {
	CRB_UNARY_OPERATOR_NONE,
	CRB_UNARY_OPERATOR_MINUS,
	CRB_UNARY_OPERATOR_INVERT,
};

struct crb_expression;

struct crb_function_call_expression {
	const char *function_name;
	const struct crb_trunk arguments;
};

struct crb_binary_expression {
	const int binary_operator;
	const struct crb_expression *left, *right;
};

struct crb_assign_expression {
	const char *variable;
	const struct crb_expression *exprand;
};

struct crb_unary_expression {
	const int unary_operator;
	const struct crb_expression *expression;
};

struct crb_expression {
	int type;
	union {
		int boolean_value;
		int int_value;
		double double_value;
		struct crb_string string_value;
		struct crb_function function_value;
		const char *identifier;
		struct crb_function_call_expression function_call_expression;
		struct crb_binary_expression binary_expression;
		struct crb_assign_expression assign_expression;
		struct crb_unary_expression unary_expression;
	} u;
};

#define crb_expression_type_is_valid(_t_) ((_t_) > CRB_EXPRESSION_NONE\
		&& (_t_) <= CRB_UNARY_EXPRESSION)

#define crb_is_valid_binary_operator(_o_) (\
		(_o_) > CRB_BINARY_OPERATOR_NONE\
		&& (_o_) <= CRB_BINARY_OPERATOR_LOGICAL_OR)

#define crb_is_valid_unary_operator(_o_) (\
		(_o_) > CRB_UNARY_OPERATOR_NONE\
		&& (_o_) <= CRB_UNARY_OPERATOR_INVERT)

#define crb_is_comparison_operator(_o_) (\
		(_o_) == CRB_BINARY_OPERATOR_GT\
		|| (_o_) == CRB_BINARY_OPERATOR_GE\
                || (_o_) == CRB_BINARY_OPERATOR_LT\
                || (_o_) == CRB_BINARY_OPERATOR_LE)

#define crb_is_equality_operator(_o_) (\
		(_o_) == CRB_BINARY_OPERATOR_EQ\
		|| (_o_) == CRB_BINARY_OPERATOR_NE)

extern struct crb_expression *crb_create_expression(int type, void *value);
extern void crb_expression_free(struct crb_expression **pexp);

#define crb_create_int_expression(_v_) ({\
	int v = (_v_);\
	crb_create_expression(CRB_INT_EXPRESSION, &v);\
})

#define crb_create_boolean_expression(_v_) ({\
	int v = (_v_);\
	crb_create_expression(CRB_BOOLEAN_EXPRESSION, &v);\
})

#define crb_create_double_expression(_v_) ({\
	double v = (_v_);\
	crb_create_expression(CRB_DOUBLE_EXPRESSION, &v);\
})

#define crb_create_string_expression(_v_) ({\
	struct crb_string v = (_v_);\
	crb_create_expression(CRB_STRING_EXPRESSION, &v);\
})

#define crb_create_identifier_expression(_v_) ({\
	const char *v = (_v_);\
	crb_create_expression(CRB_IDENTIFIER_EXPRESSION, &v);\
})

#define crb_create_function_expression(_v_) ({\
	struct crb_function _v = (_v_);\
	crb_create_expression(CRB_FUNCTION_EXPRESSION, &_v);\
})

extern struct crb_expression *crb_create_function_call_expression(
		const char *function,
		const struct crb_trunk *arguments);

extern struct crb_expression *crb_create_binary_expression(int opr,
		const struct crb_expression *left,
		const struct crb_expression *right);

extern struct crb_expression *crb_create_assign_expression(const char *variable,
		const struct crb_expression *exprand);

extern struct crb_expression *crb_create_unary_expression(int opr,
		const struct crb_expression *exp);

#endif // CRB_EXPRESSION_H

