#include "crb_interpreter.h"
#include "crb_type.h"
#include "crb_statement.h"

extern struct crb_value crb_exec_statements(
		struct crb_interpreter *itp,
		const struct crb_trunk *statements);
