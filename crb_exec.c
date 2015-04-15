#include "crb_exec.h"
#include "crb_eval_exp.h"
#include "util/crb_util.h"
#include <stdio.h>
#include <assert.h>

static struct crb_statement_result exec_statement(
		struct crb_interpreter *itp,
		struct crb_statement *statement)
{
	printf("%s statement: %p %d\n", __func__, statement, statement->type);

	struct crb_statement_result result = {0};

	switch (statement->type) {
	case CRB_EXP_STATEMENT:
		result.value = crb_eval_exp(itp, statement->u.exp_statement.expression);
		break;
	case CRB_IF_STATEMENT:
	{
		struct crb_if_statement *if_statement = &statement->u.if_statement;
		struct crb_value condition_result = crb_eval_exp(itp, if_statement->condition);
		if (!crb_is_boolean_value(condition_result)) {
			assert(0);
		}
		
		if (condition_result.u.boolean_value != 0) {
			result = crb_exec_statements(itp, &if_statement->main_statements);
			break;
		}

		switch (if_statement->else_branch.type) {
		case CRB_ELSE_BRANCH:
			result = crb_exec_statements(itp, &if_statement->else_branch.u.else_statements);
			break;
		case CRB_ELSE_IF_BRANCH:
			result = exec_statement(itp, if_statement->else_branch.u.else_if_statement);
			break;		
		default:
			assert(0);
			break;
		}
	}
		break;
	case CRB_RETURN_STATEMENT:
		result.is_return_statement = 1;
		result.value = crb_eval_exp(itp, statement->u.return_statement.expression);
		break;
	default:
		break;
	}

	printf("\t result: ");
	if (result.is_return_statement) {
		printf("<return>");
	}
	crb_value_print(result.value);
	printf("\n");

	return result;
}

struct crb_statement_result crb_exec_statements(
		struct crb_interpreter *itp,
		const struct crb_stack *statements)
{
	crb_assert(itp != NULL && statements != NULL && statements->count > 0,
			crb_do_nothing);

	struct crb_statement_result result = {0};
	struct crb_statement *statement = NULL;
	for (int i = 0; i < statements->count; ++i) {
		statement = ((struct crb_statement **)statements->data)[i];
		result = exec_statement(itp, statement);
		if (result.is_return_statement != 0) {
			break;
		}
	}

	return result;
}
