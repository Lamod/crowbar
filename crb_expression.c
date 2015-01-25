#include <stdio.h>
#include "crb_expression.h"
#include "util/crb_util.h"

static char *exp_type_desc[] = {
	"NONE", "INT", "DOUBLE", "STRING",
	"IDENTIFIER", "BINARY", "ASSIGN", "MINUS",
};

static char *binary_operator_desc[] = {
	"NONE", "ADD", "SUB", "MUL", "DIV", "MOD", "GT",
	"GE", "LT", "LE", "EQ", "NE", "LOGICAL_ADD", "LOGICAL_OR",
};

struct crb_expression *crb_create_expression(int type, void *value)
{
	crb_assert(type <= CRB_ASSIGN_EXPRESSION, return NULL);

	struct crb_expression *e = malloc(sizeof(struct crb_expression));
	if (e == NULL) {
		return NULL;
	}

	e->type = type;

	#define V(_t_) (*((_t_ *)value))
	#define SETV(_f_) (e->u._f_ = V(typeof(e->u._f_)))

	printf("%s t:%s v:", __func__, exp_type_desc[type]);

	switch (type) {
	case CRB_INT_EXPRESSION:
	{
		printf("%d\n", V(int));
		SETV(int_value);
	}
		break;
	case CRB_DOUBLE_EXPRESSION:
	{
		printf("%lf\n", V(double));
		SETV(double_value);
	}
		break;
	case CRB_STRING_EXPRESSION:
	{
		printf("%s\n", (V(struct crb_string)).data);
		SETV(string_value);
	}
		break;
	case CRB_BINARY_EXPRESSION:
	{
		int opr = ((struct crb_binary_expression *)value)->binary_operator;
		printf("<%s>\n", binary_operator_desc[opr]);
		SETV(binary_expression);
	}
		break;
	case CRB_ASSIGN_EXPRESSION:
	{
		printf("unknown\n");
		SETV(assign_expression);
	}
		break;
	case CRB_MINUS_EXPRESSION:
	{
		printf("unknown\n");
		SETV(minus_expression);
	}
		break;
	case CRB_IDENTIFIER_EXPRESSION:
	{
		printf("%s\n", V(char *));
		SETV(identifier);
	}
	default:
		assert(0);
		break;
	}

	return e;
}

struct crb_expression *crb_create_binary_expression(int opr,
		const struct crb_expression *left,
		const struct crb_expression *right)
{
	crb_assert(opr <= CRB_BINARY_OPERATOR_LOGICAL_OR, return NULL);
	crb_assert(left != NULL && right != NULL, return NULL);

	struct crb_binary_expression be = {
		.binary_operator = opr,
		.left = left,
		.right = right
	};

	return crb_create_expression(CRB_BINARY_EXPRESSION, &be);
}

struct crb_expression *crb_create_assign_expression(char *variable,
		const struct crb_expression *exprand)
{
	crb_assert(variable != NULL && exprand != NULL, return NULL);

	struct crb_assign_expression ae = {
		.variable = variable,
		.exprand = exprand
	};

	return crb_create_expression(CRB_ASSIGN_EXPRESSION, &ae);
}

struct crb_expression *crb_create_minus_expression(const struct crb_expression *exp)
{
	crb_assert(exp != NULL, return NULL);

	struct crb_minus_expression me = {
		.expression = exp
	};

	return crb_create_expression(CRB_MINUS_EXPRESSION, &me);
}
