#include "crb_type.h"
#include "util/crb_util.h"
#include "crb_statement.h"
#include "crb_struct.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int crb_string_init(struct crb_string *str, size_t capacity)
{
	return crb_stack_init(str, sizeof(char), capacity);
}

int crb_string_append_char(struct crb_string *str, char c)
{
	return crb_stack_append(str, &c, 1);
}

size_t crb_string_append_chars(struct crb_string *str, char *chars)
{
	int r = crb_stack_append(str, chars, strlen(chars));

	if (r > 0 && str->count == str->capacity) {
		crb_string_append_char(str, '\0');
	}

	return r;
}

struct crb_value CRB_NULL = (struct crb_value){0};

void crb_value_destroy(struct crb_value *v)
{
	crb_assert(v, return);

	switch(v->type) {
	case CRB_STRING_VALUE:
		free(v->u.string_value.data);
		return;
	case CRB_INSTANCE_VALUE:
		crb_instance_free(v->u.instance_value);
		return;
	case CRB_FUNCTION_VALUE:
		if (v->u.function_value.is_native_function) {
			return;
		}

		struct crb_script_function *sf = &v->u.function_value.u.script_function;
		crb_stack_destroy(&sf->parameters);
		crb_stack_destroy(&sf->block.statements);
		return;
	default:
		return;
	}
}

void crb_value_print(struct crb_value v)
{
	switch(v.type) {
	case CRB_NULL_VALUE:
		printf("null");
		break;
	case CRB_BOOLEAN_VALUE:
		printf("boolean: %s", crb_is_true(v) ? "true" : "false");
		break;
	case CRB_INT_VALUE:
		printf("int: %d", v.u.int_value);
		break;
	case CRB_DOUBLE_VALUE:
		printf("double: %f", v.u.float_value);
		break;
	case CRB_STRING_VALUE:
		printf("string: %s", v.u.string_value.data);
		break;
	case CRB_INSTANCE_VALUE:
		printf("struct");
		break;
	case CRB_FUNCTION_VALUE:
		printf("function");
		break;
	default:
		printf("unknown type: %d", v.type);
		break;
	}
}
