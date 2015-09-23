#include "crb_runtime.h"
#include "util/crb_util.h"
#include <stdio.h>

int crb_add_gloabl_function(struct crb_interpreter *itp,
		char *name, crb_native_function func)
{
	crb_assert(itp != NULL && name != NULL && func != NULL, return -1);

	struct crb_function f = {
		.is_native_function = 1,
		.u = {
			.native_function = func
		}
	};
	struct crb_value v = {
		.type = CRB_FUNCTION_VALUE,
		.u = {
			.function_value = f
		}
	};

	return crb_interpreter_set_global_variable(itp, name, v);
}

int crb_setup_interpreter(struct crb_interpreter *itp)
{
	crb_assert(itp != NULL, return -1);

	return crb_add_gloabl_function(itp, "print", crb_print);
}

struct crb_value crb_print(void *self, int argc, struct crb_value *args)
{
	for (int i = 0; i < argc; ++i) {
		crb_value_print(args[i]);
		printf(" ");
	}

	printf("\n");

	struct crb_value v = {
		.type = CRB_INT_VALUE,
		.u = {
			.int_value = argc
		}
	};

	return v;
}
