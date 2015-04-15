#include "crb_interpreter.h"
#include "crb_type.h"
#include "crb_statement.h"

struct crb_statement_result {
	struct crb_value value;
	int is_return_statement;
};

extern struct crb_statement_result crb_exec_statements(
		struct crb_interpreter *itp,
		const struct crb_stack *statements);
