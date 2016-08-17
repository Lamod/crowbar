#ifndef CRB_INTERPRETER_H
#define CRB_INTERPRETER_H

#include "util/crb_stack.h"
#include "crb_type.h"
#include "crb_struct.h"
#include "crb_statement.h"

struct crb_variable {
	const char *identifier;
	struct crb_value value;
};

struct crb_scope {
	struct crb_stack variables;
	struct crb_scope *next;
};

struct crb_interpreter {
	struct crb_block global_block;
	struct crb_stack structs;
	struct crb_scope global_scope, *top_scope;
};

extern int crb_scope_push_variable(struct crb_scope *scope,
		const char *name, struct crb_value value);
extern struct crb_variable *crb_scope_get_variable(struct crb_scope *scope,
		const char *name, int deep);
extern struct crb_value crb_scope_get_value(struct crb_scope *scope,
		const char *name, int deep);

extern int crb_interpreter_push_struct(struct crb_interpreter *itp,
		struct crb_struct *strct);
extern struct crb_struct *crb_interpreter_get_struct(
		struct crb_interpreter *itp, const char *name);

extern struct crb_interpreter *crb_create_interpreter(void);
extern void crb_interpreter_free(struct crb_interpreter **pitp);

extern void crb_interpreter_run(struct crb_interpreter *itp);

extern struct crb_scope *crb_interpreter_pop_scope(struct crb_interpreter *itp);
extern struct crb_scope *crb_interpreter_push_scope(struct crb_interpreter *itp);

extern int crb_interpreter_set_global_variable(struct crb_interpreter *itp,
		const char *name,
		struct crb_value value);
extern struct crb_value crb_interpreter_get_global_variable(struct crb_interpreter *itp,
		const char *name);

#endif //CRB_INTERPRETER_H
