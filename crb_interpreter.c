#include "crb_interpreter.h"
#include "crb_eval_exp.h"
#include "crb_exec.h"
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
	for (int i = scope->variables.count - 1; i >= 0; --i) {
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

	return crb_stack_append(&scope->variables, &var, 1) == 1 ? 0 : 1;
}

struct crb_value crb_scope_get_variable(struct crb_scope *scope,
		const char *variable, int deep)
{
	crb_assert(scope != NULL && variable != NULL, return CRB_NULL);

	struct crb_variable *pvar = NULL;
	for (int i = scope->variables.count - 1; i >= 0; --i) {
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

	crb_stack_init(&itp->global_scope, sizeof(struct crb_variable), 50);

	return itp;
}

void crb_interpreter_free(struct crb_interpreter **pitp)
{
	if (pitp == NULL) {
		return;
	}

	struct crb_interpreter *itp = *pitp;
	if (itp == NULL) {
		return;
	}

	//statements
	
	crb_stack_destroy(&itp->global_block.statements);

	// scopes
	
	struct crb_scope *scope = NULL;
	while (itp->top_scope != &itp->global_scope) {
		scope = crb_interpreter_pop_scope(itp);
		crb_stack_destroy(scope);
		free(scope);
	}
	crb_stack_destroy(&itp->global_scope);

	free(itp);
	*pitp = NULL;
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
	crb_stack_init(&s->variables, sizeof(struct crb_variable), 10);

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

void crb_interpreter_run(struct crb_interpreter *itp)
{
	crb_assert(itp != NULL, crb_do_nothing);

	crb_stack_return_if_empty(&itp->global_block.statements);

	crb_exec_block(itp, &itp->global_block);
}
