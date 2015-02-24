#ifndef CRB_INTERPRETER_H
#define CRB_INTERPRETER_H

#include "util/crb_trunk.h"
#include "crb_type.h"
#include "crb_statement.h"

struct crb_variable {
	const char *identifier;
	struct crb_value value;
};

struct crb_interpreter {
	struct crb_trunk global_variables, statements;
};

extern struct crb_interpreter *crb_create_interpreter(void);
extern struct crb_value crb_interpreter_exec(struct crb_interpreter *itp);

extern int crb_interpreter_set_global_variable(struct crb_interpreter *itp,
		const char *variable,
		struct crb_value value);
extern struct crb_value crb_interpreter_get_global_variable(struct crb_interpreter *itp,
		const char *variable);

#endif //CRB_INTERPRETER_H
