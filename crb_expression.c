#include <stdio.h>
#include "crb_expression.h"
#include "util/crb_util.h"
#include "util/crb_stack.h"
#include "crb_statement.h"

static char *exp_type_desc[] = {
	"NONE", "NULL", "BOOL", "INT", "DOUBLE", "STRING", "FUNCTION",
	"IDENTIFIER", "FUNCTION_CALL", "BINARY", "ASSIGN", "UNARY",
};

static char *binary_operator_desc[] = {
	"NONE", "ADD", "SUB", "MUL", "DIV", "MOD", "GT",
	"GE", "LT", "LE", "EQ", "NE", "LOGICAL_ADD", "LOGICAL_OR",
};

static char *unary_operator_desc[] = {
	"NONE", "MINUS", "INVERT"
};

struct crb_expression *crb_create_expression(int type, void *value)
{
	printf("%s %d %p\n", __func__, type, value);
	crb_assert(crb_expression_type_is_valid(type), return NULL);

	struct crb_expression *e = malloc(sizeof(struct crb_expression));
	if (e == NULL) {
		return NULL;
	}

	e->type = type;

#define V(_t_) (*((_t_ *)value))
#define SETV(_f_) (e->u._f_ = V(typeof(e->u._f_)))

	printf("%s t:%s v:", __func__, exp_type_desc[type]);

	switch (type) {
	case CRB_NULL_EXPRESSION:
	{
		printf("null\n");
	}
		break;
	case CRB_BOOLEAN_EXPRESSION:
	{
		printf("%d\n", V(int));
		SETV(boolean_value);
	}
		break;
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
		struct crb_assign_expression *ae = (struct crb_assign_expression *)value;
		printf("%s = <%p>\n", ae->variable, ae->exprand);
		SETV(assign_expression);
	}
		break;
	case CRB_UNARY_EXPRESSION:
	{
		int opr = ((struct crb_unary_expression *)value)->unary_operator;
		printf("<%s>\n", unary_operator_desc[opr]);
		SETV(unary_expression);
	}
		break;
	case CRB_IDENTIFIER_EXPRESSION:
	{
		printf("%s\n", V(const char *));
		SETV(identifier);
	}
		break;
	case CRB_FUNCTION_EXPRESSION:
	{
		printf("\n");
		SETV(function_value);
	}
		break;
	case CRB_FUNCTION_CALL_EXPRESSION:
	{
		printf("%s()\n", ((struct crb_function_call_expression *)value)->function_name);
		SETV(function_call_expression);
	}
		break;
	default:
		assert(0);
		break;
	}

#undef V
#undef SETV

	return e;
}

void crb_expression_free(struct crb_expression **pexp)
{
	if (pexp == NULL) {
		return;
	}

	struct crb_expression *exp = *pexp;
	if (exp == NULL) {
		return;
	}

	switch (exp->type) {
	case CRB_FUNCTION_EXPRESSION:
	{
		struct crb_function *f = &exp->u.function_value;
		if (!f->is_native_function) {
			crb_stack_destroy(&f->u.script_function.block.statements);
			crb_stack_destroy(&f->u.script_function.parameters);
		}
	}
		break;
	case CRB_IDENTIFIER_EXPRESSION:
		break;
	case CRB_FUNCTION_CALL_EXPRESSION:
		break;
	case CRB_BINARY_EXPRESSION:
		crb_expression_free(&exp->u.binary_expression.left);
		crb_expression_free(&exp->u.binary_expression.right);
		break;
	case CRB_ASSIGN_EXPRESSION:
		crb_expression_free(&exp->u.assign_expression.exprand);
		break;
	case CRB_UNARY_EXPRESSION:
		crb_expression_free(&exp->u.unary_expression.expression);
		break;
	default:
		break;
	}

	free(exp);
	*pexp = NULL;
}

struct crb_expression *crb_create_function_call_expression(
		const char *function,
		const struct crb_stack *arguments)
{
	crb_assert(function != NULL, return NULL);

	struct crb_function_call_expression fe = {
		.function_name = function,
		.arguments = arguments ? *arguments : (struct crb_stack){0}
	};

	return crb_create_expression(CRB_FUNCTION_CALL_EXPRESSION, &fe);
}

struct crb_expression *crb_create_binary_expression(int opr,
		const struct crb_expression *left,
		const struct crb_expression *right)
{
	crb_assert(crb_is_valid_binary_operator(opr), return NULL);
	crb_assert(left != NULL && right != NULL, return NULL);

	struct crb_binary_expression be = {
		.binary_operator = opr,
		.left = left,
		.right = right
	};

	return crb_create_expression(CRB_BINARY_EXPRESSION, &be);
}

struct crb_expression *crb_create_assign_expression(const char *variable,
		const struct crb_expression *exprand)
{
	crb_assert(variable != NULL && exprand != NULL, return NULL);

	struct crb_assign_expression ae = {
		.variable = variable,
		.exprand = exprand
	};

	return crb_create_expression(CRB_ASSIGN_EXPRESSION, &ae);
}

struct crb_expression *crb_create_unary_expression(int opr,
		const struct crb_expression *exp)
{
	crb_assert(exp != NULL, return NULL);
	crb_assert(crb_is_valid_unary_operator(opr), return NULL);

	struct crb_unary_expression ue = {
		.unary_operator = opr,
		.expression = exp
	};

	return crb_create_expression(CRB_UNARY_EXPRESSION, &ue);
}
