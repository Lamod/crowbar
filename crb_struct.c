#include "crb_struct.h"
#include <string.h>

int crb_struct_init(struct crb_struct *strct,
		const char *name, struct crb_stack *fields)
{
	strct->name = name;
	memcpy(&(strct->fields), fields, sizeof(struct crb_stack));
	return 0;
}

void crb_struct_destroy(struct crb_struct *strct)
{
	free((void *)strct->name);
	crb_stack_destroy(&(strct->fields));
}

const char *crb_struct_get_field(struct crb_struct *strct, int index)
{
	return crb_stack_get_element(&(strct->fields), const char **, index);
}

int crb_struct_get_index(struct crb_struct *strct,
		const char *field)
{
	if (strct == NULL || field == NULL) {
		return -1;
	}

	const char *f = NULL;
	struct crb_stack *fs = &(strct->fields);
	for (unsigned int i = 0; i < fs->count; ++i) {
		f = crb_struct_get_field(strct, i);
		if (strcmp(f, field) == 0) {
			return i;
		}
	}

	return -1;
}

struct crb_instance *crb_instance_new(struct crb_struct *strct) {
	int c = strct->fields.count;
	size_t ms = c * sizeof(struct crb_value *);
	size_t l = sizeof(struct crb_instance) + ms;
	struct crb_instance *instance = (struct crb_instance *)malloc(l);
	if (instance == NULL) {
		return NULL;
	}

	instance->definition = strct;
	instance->count = c;
	memset(instance->members, 0, ms);

	return instance;
}

void crb_instance_free(struct crb_instance *instance) {
	for (int i = 0; i < instance->count; ++i) {
		crb_value_destroy(instance->members + i);
	}

	//free(instance);
}

struct crb_value *crb_instance_get(
		struct crb_instance *instance, const char *field) {

	struct crb_struct *def = instance->definition;
	int i = crb_struct_get_index(def, field);
	if (i < 0) {
		return &CRB_NULL;
	}

	return instance->members + i;
}	

void crb_instance_set(
		struct crb_instance *instance, const char *field,
		struct crb_value *value)
{
	struct crb_struct *def = instance->definition;
	int i = crb_struct_get_index(def, field);
	if (i < 0) {
		return;
	}

	// TODO: GC
	
	instance->members[i] = *value;
}

