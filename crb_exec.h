#include "crb_interpreter.h"
#include "crb_type.h"
#include "crb_statement.h"

struct crb_block_result {
	struct crb_value value;
	int is_return_statement;
};

extern struct crb_block_result crb_exec_block(
		struct crb_interpreter *itp,
		const struct crb_block *block);
