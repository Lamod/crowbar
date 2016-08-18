#include "crb_eval_exp.h"
#include "crb_exec.h"
#include "crb_struct.h"
#include "util/crb_util.h"
#include <stdio.h>
#include <assert.h>

#define expand_binary_numberical_exp(_l_, _r_, _o_) ({\
	struct crb_value _l = (_l_), _r = (_r_), _v = CRB_NULL;\
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

	return v;
}

static struct crb_value eval_assign_exp(struct crb_interpreter *itp,
		const struct crb_assign_expression *exp)
{
	struct crb_variable *var = crb_scope_get_variable(itp->top_scope, exp->variable, 1);
	assert(var != NULL);

	var->value = crb_eval_exp(itp, exp->exprand);
	
	return var->value;
}

static struct crb_value eval_identifier_exp(struct crb_interpreter *itp,
		const char *identifier)
{
	struct crb_value v = crb_scope_get_value(itp->top_scope, identifier, 1);

	if (crb_is_null(v)) {
		assert(0);
	}

	return v;
}

static struct crb_value eval_function_call_exp(
		struct crb_interpreter *itp,
		const struct crb_function_call_expression *fe)
{
	struct crb_value func_value = crb_scope_get_value(
			itp->top_scope,
			fe->function_name, 1);
	if (!crb_is_function_value(func_value)) {
		assert(0);
	}

	struct crb_function *func = &(func_value.u.function_value);

	struct crb_stack args = {0};
	crb_stack_init(&args, sizeof(struct crb_value), fe->arguments.count);

	struct crb_expression *exp = NULL;
	struct crb_value v = CRB_NULL;
	for (int i = 0; i < fe->arguments.count; ++i) {
		crb_stack_read_element(&fe->arguments, &exp, i);
		v = crb_eval_exp(itp, exp);
		crb_stack_append(&args, &v, 1);
	}

	if (func->is_native_function) {
		v = func->u.native_function(NULL, args.count, args.data);
	} else {
		struct crb_script_function *sf = &func->u.script_function;
		crb_assert(sf->parameters.count == args.count, return CRB_NULL);

		struct crb_scope *scope = crb_interpreter_push_scope(itp);

		char *name = NULL;
		struct crb_value v = CRB_NULL;
		for (int i = 0; i < sf->parameters.count; ++i) {
			crb_stack_read_element(&sf->parameters, &name, i);
			crb_stack_read_element(&args, &v, i);

			crb_scope_push_variable(scope, name, v);
		}

		v = crb_exec_block(itp, &sf->block);

		if (scope != crb_interpreter_pop_scope(itp)) {
			crb_panic(return CRB_NULL);
		}

		free(scope);
	}

	crb_stack_destroy(&args);

	return v;
}

static struct crb_instance *new(struct crb_interpreter *itp,
		const struct crb_new_expression *ne)
{
	crb_assert(itp != NULL && ne != NULL, return NULL);
	crb_assert(ne != NULL, return NULL);

	const struct crb_struct *strct = crb_interpreter_get_struct(itp, ne->struct_name);
	crb_assert(strct != NULL, return NULL);

	struct crb_instance *instance = crb_instance_new(strct);
	if (instance == NULL) {
		return NULL;
	}

	struct crb_expression *e = NULL;
	for (unsigned int i = 0; i < ne->arguments.count; ++i) {
		e = crb_stack_get_element(&ne->arguments, struct crb_expression **, i);
		instance->members[i] = crb_eval_exp(itp, e);
	}

	return instance;
}

static struct crb_value *get_field(struct crb_interpreter *itp,
		struct crb_member_expression *me) {

	struct crb_value instance = crb_eval_exp(itp, me->instance);
	crb_assert(instance.type == CRB_MEMBER_EXPRESSION, return NULL);

	return crb_instance_get(instance.u.instance_value, me->field);
}

struct crb_value crb_eval_exp(struct crb_interpreter *itp,
		const struct crb_expression *exp)
{
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
	case CRB_NEW_EXPRESSION:
		v.type = CRB_INSTANCE_VALUE;
		v.u.instance_value = new(itp, &exp->u.new_expression);
		break;
	case CRB_MEMBER_EXPRESSION:
		v = *get_field(itp, &exp->u.member_expression);
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
