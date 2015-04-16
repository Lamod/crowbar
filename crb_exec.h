#include "crb_interpreter.h"
#include "crb_type.h"
#include "crb_statement.h"

extern struct crb_value crb_exec_block(
		struct crb_interpreter *itp,
		const struct crb_block *block);
