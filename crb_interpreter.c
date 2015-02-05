#include "crb_interpreter.h"


static struct crb_value exec_statement(struct crb_interpreter *itp,
		struct crb_statement *statement)
{
	switch (statement->type) {
	case CRB_EXP_STATEMENT:
	{
		return eval_expression(itp,
				statement->u.exp_statement->expression);
	}
	break;
	default:
	break;
	}

	return CRB_NULL;
}

struct crb_value crb_interpreter_exec(struct crb_interpreter *itp)
{
	return CRB_NULL;
}
