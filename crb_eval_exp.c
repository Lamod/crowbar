#include "crb_eval_exp.h"
#include "crb_exec.h"
#include "util/crb_util.h"
#include <stdio.h>
#include <assert.h>

#define expand_binary_numberical_exp(_l_, _r_, _o_) ({\
	struct crb_value _l = (_l_), _r = (_r_), _v = CRB_NULL;\
	printf("%s (", __func__);\
	crb_value_print(_l);\
	printf(")");\
	printf(" %s ", #_o_);\
	printf("(");\
	crb_value_print(_r);\
	printf(")");\
	\
	if (_l.type == CRB_DOUBLE_VALUE) {\
		_v.type = CRB_DOUBLE_VALUE;\
		if (_r.type == CRB_DOUBLE_VALUE) {\
			_v.u.float_value =\
				_l.u.float_value _o_ _r.u.float_value;\
		} else {\
			_v.u.float_value =\
				_l.u.float_value _o_ _r.u.int_value;\
		}\
	} else if (_r.type == CRB_DOUBLE_VALUE) {\
		_v.type = CRB_DOUBLE_VALUE;\
		_v.u.float_value = _l.u.int_value _o_ _r.u.float_value;\
	} else {\
		_v.type = CRB_INT_VALUE;\
		_v.u.int_value = _l.u.int_value _o_ _r.u.int_value;\
	}\
	printf(" = (");\
	crb_value_print(_v);\
	printf(")\n");\
	\
	_v;\
})

#define expand_unary_exp(_v_, _o_) ({\
	struct crb_value _v = (_v_);\
	if (_v.type == CRB_DOUBLE_VALUE) {\
		_v.u.float_value = _o_ _v.u.float_value;\
	} else if (_v.type == CRB_INT_VALUE) {\
		_v.u.int_value = _o_ _v.u.int_value;\
	} else if (_v.type == CRB_BOOLEAN_VALUE) {\
		_v.u.boolean_value = _o_ _v.u.boolean_value;\
	}\
	_v;\
})

static struct crb_value boolean_convert(struct crb_value v)
{
	crb_assert(crb_is_numberical_value(v), return CRB_NULL);

	if (v.type == CRB_INT_VALUE) {
		v.u.boolean_value = (v.u.int_value != 0);
	} else {
		v.u.boolean_value = (v.u.float_value != .0);
	}

	v.type = CRB_BOOLEAN_VALUE;

	return v;
}

static struct crb_value eval_comparision_exp(struct crb_interpreter *itp,
		struct crb_value left,
		struct crb_value right,
		int operator)
{
	crb_assert(crb_is_comparison_operator(operator), return CRB_NULL);
	if (!crb_is_numberical_value(left)) {
		//TODO:runtime error
		assert(0);
	}
	if (!crb_is_numberical_value(right)) {
		//TODO:runtime error
		assert(0);
	}

	struct crb_value v = CRB_NULL;

	switch(operator) {
	case CRB_BINARY_OPERATOR_GT:
		v = expand_binary_numberical_exp(left, right, >);
		break;
	case CRB_BINARY_OPERATOR_GE:
		v = expand_binary_numberical_exp(left, right, >=);
		break;
	case CRB_BINARY_OPERATOR_LE:
		v = expand_binary_numberical_exp(left, right, <=);
		break;
	case CRB_BINARY_OPERATOR_LT:
		v = expand_binary_numberical_exp(left, right, <);
		break;
	}

	return boolean_convert(v);
}

static struct crb_value eval_equality_exp(struct crb_interpreter *itp,
		struct crb_value left,
		struct crb_value right,
		int operator)
{
	crb_assert(crb_is_equality_operator(operator), return CRB_NULL);

	struct crb_value v = CRB_NULL;

	if (crb_is_numberical_value(left) && crb_is_numberical_value(right)) {
		if (operator == CRB_BINARY_OPERATOR_EQ) {
			v = expand_binary_numberical_exp(left, right, ==);
		} else {
			v = expand_binary_numberical_exp(left, right, !=);
		}

		v = boolean_convert(v);
	} else if (crb_is_boolean_value(left) && crb_is_boolean_value(right)) { // both boolean exp
		v.type = CRB_BOOLEAN_VALUE;
		if (operator == CRB_BINARY_OPERATOR_EQ) {
			v.u.boolean_value =
				(left.u.boolean_value == right.u.boolean_value);
		} else {
			v.u.boolean_value =
				left.u.boolean_value != right.u.boolean_value;
		}
	} else {
		assert(0);
	}

	printf("%s ", __func__);
	crb_value_print(v);
	printf("\n");
	return v;
}

static struct crb_value eval_logical_exp(struct crb_interpreter *itp,
		struct crb_value left,
		struct crb_value right,
		int operator)
{
	crb_assert(operator == CRB_BINARY_OPERATOR_LOGICAL_OR
			|| operator == CRB_BINARY_OPERATOR_LOGICAL_AND,
			return CRB_NULL);

	if (!crb_is_boolean_value(left) || !crb_is_boolean_value(right)) {
		assert(0);
	}

	struct crb_value v = {.type = CRB_BOOLEAN_VALUE};

	if (operator == CRB_BINARY_OPERATOR_LOGICAL_OR) {
		v.u.boolean_value = left.u.boolean_value || right.u.boolean_value;
	} else {
		v.u.boolean_value = left.u.boolean_value || right.u.boolean_value;
	}

	printf("%s ", __func__);
	crb_value_print(v);
	printf("\n");

	return v;
}

static struct crb_value eval_numberical_exp(struct crb_interpreter *itp,
		struct crb_value left,
		struct crb_value right,
		int operator)
{
	if (!crb_is_numberical_value(left) || !crb_is_numberical_value(right)) {
		assert(0);
	}

	struct crb_value v = CRB_NULL;

	switch (operator) {
	case CRB_BINARY_OPERATOR_ADD:
		v = expand_binary_numberical_exp(left, right, +);
		break;
	case CRB_BINARY_OPERATOR_SUB:
		v = expand_binary_numberical_exp(left, right, -);
		break;
	case CRB_BINARY_OPERATOR_MUL:
		v = expand_binary_numberical_exp(left, right, *);
		break;
	case CRB_BINARY_OPERATOR_DIV:
		v = expand_binary_numberical_exp(left, right, /);
		break;
	case CRB_BINARY_OPERATOR_MOD:
		if (right.type != CRB_INT_VALUE || left.type != CRB_INT_VALUE) {
			//TODO:runtime error
			assert(0);
		}

		v.type = CRB_INT_VALUE;
		v.u.int_value = left.u.int_value % right.u.int_value;
		break;
	}

	return v;
}
	
static struct crb_value eval_binary_exp(struct crb_interpreter *itp,
		const struct crb_binary_expression *exp)
{
	crb_assert(crb_is_valid_binary_operator(exp->binary_operator),
			return CRB_NULL);

	struct crb_value r = crb_eval_exp(itp, exp->right),
			 l = crb_eval_exp(itp, exp->left),
			 v = CRB_NULL;

	switch (exp->binary_operator) {
	case CRB_BINARY_OPERATOR_ADD:
	case CRB_BINARY_OPERATOR_SUB:
	case CRB_BINARY_OPERATOR_MUL:
	case CRB_BINARY_OPERATOR_DIV:
	case CRB_BINARY_OPERATOR_MOD:
		v = eval_numberical_exp(itp, l, r, exp->binary_operator);
		break;
	case CRB_BINARY_OPERATOR_GT:
	case CRB_BINARY_OPERATOR_GE:
	case CRB_BINARY_OPERATOR_LE:
	case CRB_BINARY_OPERATOR_LT:
		v = eval_comparision_exp(itp, l, r, exp->binary_operator);
		break;
	case CRB_BINARY_OPERATOR_NE:
	case CRB_BINARY_OPERATOR_EQ:
		v = eval_equality_exp(itp, l, r, exp->binary_operator);
		break;
	case CRB_BINARY_OPERATOR_LOGICAL_OR:
	case CRB_BINARY_OPERATOR_LOGICAL_AND:
		v = eval_logical_exp(itp, l, r, exp->binary_operator);
		break;
	default:
		break;
	}

	return v;
}

static struct crb_value eval_unary_exp(struct crb_interpreter *itp,
		const struct crb_unary_expression *exp)
{
	crb_assert(crb_is_valid_unary_operator(exp->unary_operator),
				return CRB_NULL);

	struct crb_value v = crb_eval_exp(itp, exp->expression);

	if (crb_is_null(v)) {
		assert(0);
	}
	
	switch(exp->unary_operator) {
	case CRB_UNARY_OPERATOR_INVERT:
		if (!crb_is_boolean_value(v)) {
			assert(0);
		}
		v = expand_unary_exp(v, !);
		break;
	case CRB_UNARY_OPERATOR_MINUS:
		if (!crb_is_numberical_value(v)) {
			assert(0);
		}
		v = expand_unary_exp(v, -);
		break;
	default:
		break;
	}

	printf("%s ", __func__);
	crb_value_print(v);
	printf("\n");

	return v;
}

static struct crb_value eval_assign_exp(struct crb_interpreter *itp,
		const struct crb_assign_expression *exp)
{
	struct crb_value v = crb_eval_exp(itp, exp->exprand);
	
	int r = crb_scope_set_variable(itp->top_scope, exp->variable, v);

	printf("%s %s = (", __func__, exp->variable);
	crb_value_print(v);
	printf(")\n");

	return r == 0 ? v : CRB_NULL;
}

static struct crb_value eval_identifier_exp(struct crb_interpreter *itp,
		const char *identifier)
{
	struct crb_value v = crb_scope_get_variable(itp->top_scope, identifier, 1);

	printf("%s %s = (", __func__, identifier);
	crb_value_print(v);
	printf(")\n");
	
	if (crb_is_null(v)) {
		assert(0);
	}

	return v;
}

static struct crb_value eval_function_call_exp(
		struct crb_interpreter *itp,
		const struct crb_function_call_expression *fe)
{
	struct crb_value func_value = crb_scope_get_variable(
			itp->top_scope,
			fe->function_name, 1);
	if (!crb_is_function_value(func_value)) {
		assert(0);
	}

	struct crb_function *func = &(func_value.u.function_value);
	crb_assert(func->parameters.count == fe->arguments.count,
			return CRB_NULL);

	struct crb_scope *scope = crb_interpreter_push_scope(itp);

	const char *name = NULL;
	const struct crb_expression *exp = NULL;

	for (int i = 0; i < func->parameters.count; ++i) {
		crb_stack_read_element(&func->parameters, &name, i);
		crb_stack_read_element(&fe->arguments, &exp, i);

		crb_scope_set_variable(scope, name, crb_eval_exp(itp, exp));
	}

	struct crb_value r = crb_exec_statements(itp, &func->statements).value;

	if (scope != crb_interpreter_pop_scope(itp)) {
		crb_panic(return CRB_NULL);
	}
	
	free(scope);

	return r;
}

struct crb_value crb_eval_exp(struct crb_interpreter *itp,
		const struct crb_expression *exp)
{
	printf("%s %p %p %d\n", __func__, itp, exp, exp->type);
	crb_assert(itp != NULL && exp != NULL, return CRB_NULL);
	crb_assert(crb_expression_type_is_valid(exp->type),
			return CRB_NULL);

	struct crb_value v = CRB_NULL;

	switch (exp->type) {
	case CRB_BOOLEAN_EXPRESSION:
		v.type = CRB_BOOLEAN_VALUE;
		v.u.boolean_value = exp->u.boolean_value;
		break;
	case CRB_INT_EXPRESSION:
		v.type = CRB_INT_VALUE;
		v.u.int_value = exp->u.int_value;
		break;
	case CRB_DOUBLE_EXPRESSION:
		v.type = CRB_DOUBLE_VALUE;
		v.u.float_value = exp->u.double_value;
		break;
	case CRB_STRING_EXPRESSION:
		v.type = CRB_STRING_VALUE;
		v.u.string_value = exp->u.string_value;
		break;
	case CRB_FUNCTION_EXPRESSION:
		v.type = CRB_FUNCTION_VALUE;
		v.u.function_value = exp->u.function_value;
		break;
	case CRB_BINARY_EXPRESSION:
		v = eval_binary_exp(itp, &exp->u.binary_expression);
		break;
	case CRB_UNARY_EXPRESSION:
		v = eval_unary_exp(itp, &exp->u.unary_expression);
		break;
	case CRB_ASSIGN_EXPRESSION:
		v = eval_assign_exp(itp, &exp->u.assign_expression);
		break;
	case CRB_IDENTIFIER_EXPRESSION:
		v = eval_identifier_exp(itp, exp->u.identifier);
		break;
	case CRB_FUNCTION_CALL_EXPRESSION:
		v = eval_function_call_exp(itp, &exp->u.function_call_expression);
		break;
	default:
		break;
	}

	return v;
}
