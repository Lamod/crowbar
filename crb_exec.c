#include "crb_exec.h"
#include "crb_eval_exp.h"
#include "util/crb_util.h"
#include <stdio.h>
#include <assert.h>

struct statement_result {
	struct crb_value value;
	int is_return_statement;
};

static struct statement_result exec_statement(
		struct crb_interpreter *itp,
		struct crb_statement *statement)
{
	printf("%s statement: %p %d\n", __func__, statement, statement->type);

	struct statement_result result = {0};

	switch (statement->type) {
	case CRB_EXP_STATEMENT:
		result.value = crb_eval_exp(itp, statement->u.exp_statement.expression);
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

struct crb_value crb_exec_statements(
		struct crb_interpreter *itp,
		const struct crb_trunk *statements)
{
	crb_assert(itp != NULL && statements != NULL && statements->count > 0,
			crb_do_nothing);

	struct statement_result r = {0};
	struct crb_statement *s = NULL;
	for (int i = 0; i < statements->count; ++i) {
		s = ((struct crb_statement **)statements->data)[i];
		r = exec_statement(itp, s);
		if (r.is_return_statement != 0) {
			break;
		}
	}

	return r.value;
}
