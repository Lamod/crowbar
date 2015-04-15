#ifndef CRB_INTERPRETER_H
#define CRB_INTERPRETER_H

#include "util/crb_trunk.h"
#include "crb_type.h"
#include "crb_statement.h"

struct crb_variable {
	const char *identifier;
	struct crb_value value;
};

struct crb_scope {
	struct crb_trunk variables;
	struct crb_scope *next;
};

struct crb_interpreter {
	struct crb_trunk statements;
	struct crb_scope global_scope, *top_scope;
};

extern int crb_scope_set_variable(struct crb_scope *scope,
		const char *variable, struct crb_value value);
extern struct crb_value crb_scope_get_variable(struct crb_scope *scope,
		const char *variable, int deep);

extern struct crb_interpreter *crb_create_interpreter(void);
extern void crb_interpreter_free(struct crb_interpreter **pitp);

extern void crb_interpreter_run(struct crb_interpreter *itp);

extern struct crb_scope *crb_interpreter_pop_scope(struct crb_interpreter *itp);
extern struct crb_scope *crb_interpreter_push_scope(struct crb_interpreter *itp);

extern int crb_interpreter_set_global_variable(struct crb_interpreter *itp,
		const char *variable,
		struct crb_value value);
extern struct crb_value crb_interpreter_get_global_variable(struct crb_interpreter *itp,
		const char *variable);

#endif //CRB_INTERPRETER_H
