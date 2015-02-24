#include "crb_interpreter.h"
#include "crb_eval_exp.h"
#include "util/crb_util.h"
#include <string.h>
#include <stdio.h>

int crb_scope_set_variable(struct crb_scope *scope,
		const char *variable, struct crb_value value)
{
	crb_assert(scope != NULL && variable != NULL, return -1);

	printf("%s scope<%p> %s = ", __func__, scope, variable);
	crb_value_print(value);
	printf("\n");

	struct crb_variable *pvar = NULL;
	for (int i = 0; i < scope->variables.count; ++i) {
		pvar = (struct crb_variable *)scope->variables.data + i;
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

	return crb_trunk_append(&scope->variables, &var, 1) == 1 ? 0 : 1;
}

struct crb_value crb_scope_get_variable(struct crb_scope *scope,
		const char *variable, int deep)
{
	crb_assert(scope != NULL && variable != NULL, return CRB_NULL);

	struct crb_variable *pvar = NULL;
	for (int i = 0; i < scope->variables.count; ++i) {
		pvar = (struct crb_variable *)scope->variables.data + i;

		if (strcmp(variable, pvar->identifier) != 0) {
			continue;
		}

		return pvar->value;
	}

	if (deep != 0 && scope->next != NULL) {
		return crb_scope_get_variable(scope->next, variable, 1);
	}

	return CRB_NULL;
}

struct crb_interpreter *crb_create_interpreter(void)
{
	struct crb_interpreter *itp = calloc(sizeof(*itp), 1);
	if (itp == NULL) {
		return NULL;
	}

	itp->top_scope = &(itp->global_scope);

	crb_trunk_init(&itp->global_scope, sizeof(struct crb_variable), 50);

	return itp;
}

int crb_interpreter_set_global_variable(struct crb_interpreter *itp,
		const char *variable,
		struct crb_value value)
{
	crb_assert(itp != NULL, return -1);

	return crb_scope_set_variable(&itp->global_scope, variable, value);
}

struct crb_value crb_interpreter_get_global_variable(struct crb_interpreter *itp,
		const char *variable)
{
	crb_assert(itp != NULL, return CRB_NULL);

	return crb_scope_get_variable(&itp->global_scope, variable, 0);
}

struct crb_scope *crb_interpreter_push_scope(struct crb_interpreter *itp) 
{
	struct crb_scope *s = calloc(sizeof(*s), 1);
	crb_trunk_init(&s->variables, sizeof(struct crb_variable), 10);

	s->next = itp->top_scope;
	itp->top_scope = s;

	return s;
}

struct crb_scope *crb_interpreter_pop_scope(struct crb_interpreter *itp)
{
	crb_assert(itp->top_scope != &(itp->global_scope), (void)0);

	struct crb_scope *s = itp->top_scope;
	itp->top_scope = s->next;

	return s;
}

static struct crb_value exec_statement(
		struct crb_interpreter *itp,
		struct crb_statement *statement)
{
	printf("%s statement: %p %d\n", __func__, statement, statement->type);

	switch (statement->type) {
	case CRB_EXP_STATEMENT:
		crb_eval_exp(itp, statement->u.exp_statement.expression);
		break;
	case CRB_RETURN_STATEMENT:
		return crb_eval_exp(itp, statement->u.return_statement.expression);
	default:
		break;
	}

	return CRB_NULL;
}

struct crb_value crb_interpreter_exec_statements(
		struct crb_interpreter *itp,
		const struct crb_trunk *statements)
{
	crb_assert(itp != NULL && statements != NULL && statements->count > 0,
			crb_do_nothing);

	struct crb_value r = CRB_NULL;
	struct crb_statement *s = NULL;
	for (int i = 0; i < statements->count; ++i) {
		s = ((struct crb_statement **)statements->data)[i];
		r = exec_statement(itp, s);
	}

	return r;
}

void crb_interpreter_run(struct crb_interpreter *itp)
{
	crb_assert(itp != NULL, crb_do_nothing);

	crb_trunk_return_if_empty(&itp->statements);

	crb_interpreter_exec_statements(itp, &itp->statements);
}
