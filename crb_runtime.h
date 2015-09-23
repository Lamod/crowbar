#ifndef CRB_RUNTIME_H
#define CRB_RUNTIME_H

#include "crb_type.h"
#include "crb_interpreter.h"

extern int crb_add_gloabl_function(struct crb_interpreter *itp,
		char *name, crb_native_function func);
extern int crb_setup_interpreter(struct crb_interpreter *itp);

extern struct crb_value crb_print(void *self, int argc, struct crb_value *args);

#endif // CRB_RUNTIME_H
