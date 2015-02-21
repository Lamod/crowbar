#include "crb_interpreter.h"
#include "crb_eval_exp.h"
#include "util/crb_util.h"
#include <string.h>
#include <stdio.h>

struct crb_interpreter *crb_create_interpreter(void)
{
	struct crb_interpreter *itp = malloc(sizeof(*itp));
	if (itp == NULL) {
		return NULL;
	}

	crb_trunk_init(&itp->global_variables, sizeof(struct crb_variable), 50);
	crb_trunk_init(&itp->statements, sizeof(struct crb_statement *), 50);

	return itp;
}

int crb_interpreter_set_global_variable(struct crb_interpreter *itp,
		const char *variable,
		struct crb_value value)
{
	crb_assert(itp != NULL && variable != NULL, return 1);

	struct crb_variable *pvar = NULL;
	for (int i = 0; i < itp->global_variables.count; ++i) {
		pvar = (struct crb_variable *)itp->global_variables.data + i;
		if (strcmp(variable, pvar->identifier) != 0) {
			continue;
		}

		pvar->value = value;
		
		return 0;
	}

	struct crb_variable var = {
		.identifier = variable,
		.value = value
	};

	return crb_trunk_append(&itp->global_variables, &var, 1) == 1 ? 0 : 1;
}

struct crb_value crb_interpreter_get_global_variable(struct crb_interpreter *itp,
		const char *variable)
{
	struct crb_variable *pvar = NULL;
	for (int i = 0; i < itp->global_variables.count; ++i) {
		pvar = (struct crb_variable *)itp->global_variables.data + i;
		if (strcmp(variable, pvar->identifier) != 0) {
			continue;
		}

		return pvar->value;
	}

	return CRB_NULL;
}

static struct crb_value exec_statement(struct crb_interpreter *itp,
		struct crb_statement *statement)
{
	switch (statement->type) {
	case CRB_EXP_STATEMENT:
	{
		return crb_eval_exp(itp,
				statement->u.exp_statement.expression);
	}
	break;
	default:
	break;
	}

	return CRB_NULL;
}

struct crb_value crb_interpreter_exec(struct crb_interpreter *itp)
{
	crb_assert(itp != NULL, return CRB_NULL);

	struct crb_statement *s = NULL;
	for (int i = 0; i < itp->statements.count; ++i) {
		s = *((struct crb_statement **)itp->statements.data + i);
		exec_statement(itp, s);
	}

	return CRB_NULL;
}
