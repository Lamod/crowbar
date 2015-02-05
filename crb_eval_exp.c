#include "crb_eval_exp.h"
#include "util/crb_util.h"
#include <stdio.h>
#include <assert.h>

#define cal_numberical_exp(_l_, _r_, _o_) ({\
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

static struct crb_value eval_binary_exp(struct crb_interpreter *itp,
		const struct crb_binary_expression *exp)
{
//	crb_assert(exp != NULL && itp != NULL, return CRB_NULL);
	crb_assert(
		crb_binary_expression_operator_is_valid(exp->binary_operator),
		return CRB_NULL
	);

	struct crb_value r = crb_eval_exp(itp, exp->right),
			 l = crb_eval_exp(itp, exp->left);

	if (!crb_is_numberical_value(r) || !crb_is_numberical_value(l)) {
		//TODO:runtime error
		assert(0);
		return CRB_NULL;
	}

	struct crb_value v = CRB_NULL;

	switch (exp->binary_operator) {
	case CRB_BINARY_OPERATOR_ADD:
	{
		v = cal_numberical_exp(l, r, +);
	}
		break;
	case CRB_BINARY_OPERATOR_SUB:
	{
		v = cal_numberical_exp(l, r, -);
	}
		break;
	case CRB_BINARY_OPERATOR_MUL:
	{
		v = cal_numberical_exp(l, r, *);
	}
		break;
	case CRB_BINARY_OPERATOR_DIV:
	{
		v = cal_numberical_exp(l, r, /);
	}
		break;
	case CRB_BINARY_OPERATOR_MOD:
	{
		if (r.type != CRB_INT_VALUE || l.type != CRB_INT_VALUE) {
			//TODO:runtime error
			assert(0);
		}

		v.type = CRB_INT_VALUE;
		v.u.int_value = l.u.int_value + r.u.float_value;
	}
		break;
	default:
		break;
	}

	return v;
}

struct crb_value crb_eval_exp(struct crb_interpreter *itp,
		const struct crb_expression *exp)
{
//	crb_assert(itp != NULL && exp != NULL, return CRB_NULL);
	crb_assert(crb_expression_type_is_valid(exp->type), return CRB_NULL);

	struct crb_value v = CRB_NULL;

	switch (exp->type) {
	case CRB_INT_EXPRESSION:
	{
		v.type = CRB_INT_VALUE;
		v.u.int_value = exp->u.int_value;
	}
		break;
	case CRB_DOUBLE_EXPRESSION:
	{
		v.type = CRB_DOUBLE_VALUE;
		v.u.float_value = exp->u.double_value;
	}
		break;
	case CRB_STRING_EXPRESSION:
	{
		v.type = CRB_STRING_EXPRESSION;
		v.u.string_value = exp->u.string_value;
	}
		break;
	case CRB_BINARY_EXPRESSION:
	{
		v = eval_binary_exp(itp, &exp->u.binary_expression);
	}
		break;
	default:
		break;
	}

	return v;
}
