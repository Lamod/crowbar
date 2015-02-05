#ifndef CRB_INTERPRETER_H
#define CRB_INTERPRETER_H

#include "crb_type.h"
#include "crb_statement.h"

struct crb_list {
	void *element;
	struct crb_list *next;
};

struct crb_interpreter {
	struct crb_list *statements;
};

extern struct crb_value crb_interpreter_exec(struct crb_interpreter *itp);

#endif //CRB_INTERPRETER_H
