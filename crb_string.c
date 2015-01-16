#include "crb_string.h"
#include "util/crb_util.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int crb_string_init(struct crb_string *str, size_t capacity)
{
	crb_assert(str != NULL, return -1);

	str->data = malloc(capacity + 1);
	if (str->data == NULL) {
		return -1;
	}

	str->data[0] = '\0';

	str->capacity = capacity;
	str->length = 0;

	return 0;
}

int crb_string_append_char(struct crb_string *str, char c)
{
	crb_assert(str != NULL, return -1);
	crb_assert(str->capacity >= str->length, return -1);

	if (str->capacity == str->length) {
		char *d = realloc(str->data, 2 * str->capacity);
		if (d == NULL) {
			return -1;
		}

		str->data = d;
		str->capacity *= 2;
	}

	str->data[str->length] = c;
	str->length += 1;
	str->data[str->length] = '\0';

	return 0;
}

size_t crb_string_append_chars(struct crb_string *str, char *chars)
{
	crb_assert(str != NULL, return -1);

	int i = 0, l = strlen(chars);
	while (i < l && crb_string_append_char(str, chars[i++]) == 0) {}
	return i;
}
