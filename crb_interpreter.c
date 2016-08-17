#include "crb_interpreter.h"
#include "crb_eval_exp.h"
#include "crb_exec.h"
#include "util/crb_util.h"
#include <string.h>
#include <stdio.h>

static void crb_variable_destroy(struct crb_variable *var)
{
	if (var == NULL) {
		return;
	}

	crb_assert(var->identifier, return);
	free(var->identifier);
	crb_value_destroy(&var->value);
}

int crb_scope_push_variable(struct crb_scope *scope,
		const char *name, struct crb_value value)
{
	if (crb_scope_get_variable(scope, name, 0)) {
		return 1;
	}

	struct crb_variable var = {
		.identifier = name,
		.value = value
	};

	return crb_stack_append(&scope->variables, &var, 1) == 1 ? 0 : -1;
}

struct crb_variable *crb_scope_get_variable(struct crb_scope *scope,
		const char *name, int deep)
{
	crb_assert(scope != NULL && name != NULL, return NULL);

	struct crb_variable *pvar = NULL;
	for (int i = scope->variables.count - 1; i >= 0; --i) {
		pvar = (struct crb_variable *)scope->variables.data + i;

		if (strcmp(name, pvar->identifier) != 0) {
			continue;
		}

		return pvar;
	}

	if (deep != 0 && scope->next != NULL) {
		return crb_scope_get_variable(scope->next, name, 1);
	}

	return NULL;
}

struct crb_value crb_scope_get_value(struct crb_scope *scope,
		const char *name, int deep)
{
	struct crb_variable *var = crb_scope_get_variable(scope, name, deep);
	if (var != NULL) {
		return var->value;
	}

	return CRB_NULL;
}

struct crb_interpreter *crb_create_interpreter(void)
{
	struct crb_interpreter *itp = calloc(sizeof(*itp), 1);
	if (itp == NULL) {
		return NULL;
	}

	crb_stack_init(&itp->global_scope, sizeof(struct crb_variable), 50);
	itp->global_scope.variables.destroy_func = crb_variable_destroy;

	itp->top_scope = &(itp->global_scope);

	crb_stack_init(&itp->structs, sizeof(struct crb_struct), 10);

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
		crb_stack_destroy(&scope->variables);
		free(scope);
	}
	crb_stack_destroy(&itp->global_scope.variables);

	// structs
	
	crb_stack_destroy(&itp->structs);

	free(itp);

	*pitp = NULL;
}

int crb_interpreter_set_global_variable(struct crb_interpreter *itp,
		const char *name,
		struct crb_value value)
{
	crb_assert(itp != NULL, return -1);

	return crb_scope_push_variable(&itp->global_scope, name, value);
}

struct crb_value crb_interpreter_get_global_variable(struct crb_interpreter *itp,
		const char *name)
{
	crb_assert(itp != NULL, return CRB_NULL);

	return crb_scope_get_value(&itp->global_scope, name, 0);
}

int crb_interpreter_push_struct(struct crb_interpreter *itp,
		struct crb_struct *strct)
{
	return crb_stack_append(&(itp->structs), strct, 1);
}

struct crb_struct *crb_interpreter_get_struct(
		struct crb_interpreter *itp, const char *name)
{
	struct crb_stack *ss = &(itp->structs);
	struct crb_struct *s = NULL;
	for (unsigned int i = 0; i < ss->count; ++i) {
		s = (typeof(s))(ss->data) + i;
		if (strcmp(s->name, name) == 0) {
			return s;
		}
	}

	return NULL;
}

struct crb_scope *crb_interpreter_push_scope(struct crb_interpreter *itp) 
{
	crb_assert(itp, return NULL);

	struct crb_scope *s = calloc(sizeof(*s), 1);
	crb_stack_init(&s->variables, sizeof(struct crb_variable), 10);
	s->variables.destroy_func = crb_variable_destroy;

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
