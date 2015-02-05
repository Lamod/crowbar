#include "crb_interpreter.h"
#include "crb_type.h"
#include "crb_expression.h"

extern struct crb_value crb_eval_exp(struct crb_interpreter *itp,
		const struct crb_expression *exp);
