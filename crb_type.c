#include "crb_type.h"
#include "util/crb_util.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int crb_string_init(struct crb_string *str, size_t capacity)
{
	return crb_trunk_init(str, sizeof(char), capacity);
}

int crb_string_append_char(struct crb_string *str, char c)
{
	return crb_trunk_append(str, &c, 1);
}

size_t crb_string_append_chars(struct crb_string *str, char *chars)
{
	int r = crb_trunk_append(str, chars, strlen(chars));

	if (r > 0 && str->count == str->capacity) {
		crb_string_append_char(str, '\0');
	}

	return r;
}

void crb_value_print(struct crb_value v)
{
	switch(v.type) {
	case CRB_NULL_VALUE:
		printf("null");
		break;
	case CRB_BOOLEAN_VALUE:
		printf("boolean: %s", v.u.boolean_value ? "true" : "false");
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
	default:
		printf("unknown type: %d", v.type);
		break;
	}
}
