#include "crb_trunk.h"
#include "crb_util.h"
#include <stdlib.h>
#include <string.h>

struct _crb_trunk {
	CRB_TRUNK_HEADER;
};

int crb_trunk_init(void *trunk, size_t e_size, unsigned int capacity)
{
	crb_assert(trunk != NULL, return 1);
	crb_assert(e_size != 0 && capacity != 0, return 1);

	struct _crb_trunk *t = (struct _crb_trunk *)trunk;

	t->data = calloc(e_size, capacity);
	if (t->data == NULL) {
		return -1;
	}

	t->e_size = e_size;
	t->count = 0;
	t->capacity = capacity;
	
	return 0;
}

unsigned int crb_trunk_append(void *trunk, void *elements, unsigned int count)
{
	crb_assert(trunk != NULL, return 0);
	crb_assert(elements != NULL && count > 0, return 0);

	struct _crb_trunk *t = (struct _crb_trunk *)trunk;

	unsigned int new_count = t->count + count;
	if (t->capacity < new_count) {
		unsigned int new_cap = 2 * (new_count);
		t->data = realloc(t->data, new_cap);
		t->capacity = new_cap;

		memset(t->data + new_count * t->e_size, 0, (t->capacity - new_count) * t->e_size);
	}

	memcpy(t->data + (t->count * t->e_size), elements, count * t->e_size);
	t->count = new_count;

	return count;
}

