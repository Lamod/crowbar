#ifndef CRB_STRUCT_H
#define CRB_STRUCT_H

#include "util/crb_stack.h"
#include "crb_type.h"

struct crb_struct {
	const char *name;
	struct crb_stack fields;
};

struct crb_instance {
	struct crb_struct *definition;

	int count;
	struct crb_value members[1];
};

extern int crb_struct_init(struct crb_struct *strct,
		const char *name, struct crb_stack *fields);
extern void crb_struct_destroy(struct crb_struct *strct);

extern int crb_struct_get_index(struct crb_struct *strct,
		const char *field);
extern const char *crb_struct_get_field(struct crb_struct *strct,
		int index);

extern struct crb_instance *crb_instance_new(struct crb_struct *strct);
extern void crb_instance_free(struct crb_instance *instance);

extern struct crb_value *crb_instance_get(
		struct crb_instance *instance, const char *field);	
extern void crb_instance_set(
		struct crb_instance *instance, const char *field,
		struct crb_value *value);

#endif // CRB_STRUCT_H
