#include "crb_trunk.h"
#include "crb_util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CRB_TRUNK(_t_) ((struct crb_trunk *)(_t_))

int crb_trunk_init(void *trunk, size_t e_size, unsigned int capacity)
{
	crb_assert(trunk != NULL, return 1);
	crb_assert(e_size != 0 && capacity != 0, return 1);

	struct crb_trunk *t = CRB_TRUNK(trunk);

	t->data = calloc(e_size, capacity);
	if (t->data == NULL) {
		return -1;
	}

	t->e_size = e_size;
	t->count = 0;
	t->capacity = capacity;
	
	return 0;
}

void crb_trunk_destroy(void *trunk)
{
	if (trunk == NULL) {
		return;
	}

	struct crb_trunk *t = CRB_TRUNK(trunk);
	if (t->data != NULL) {
		free(t->data);
	}
	memset(t, 0, sizeof(*t));
}

unsigned int crb_trunk_append(void *trunk, void *elements, unsigned int count)
{
	crb_assert(trunk != NULL, return 0);
	crb_assert(elements != NULL && count > 0, return 0);

	struct crb_trunk *t = CRB_TRUNK(trunk);

	crb_assert(t->e_size != 0, return -1);

	unsigned int new_count = t->count + count;
	if (t->capacity < new_count) {
		unsigned int new_cap = 2 * (new_count);
		t->data = realloc(t->data, new_cap * t->e_size);
		t->capacity = new_cap;

		memset(t->data + new_count * t->e_size, 0, (t->capacity - new_count) * t->e_size);
	}

	memcpy(t->data + (t->count * t->e_size), elements, count * t->e_size);
	t->count = new_count;

	return count;
}

int crb_trunk_copy(void *dest, void *src, int count)
{
	crb_assert(dest != NULL && src != NULL, return -1);

	struct crb_trunk *s = CRB_TRUNK(src), *d = CRB_TRUNK(dest);

	crb_assert(s->e_size != 0 && d->e_size != 0, return -1);

	unsigned int c = MIN(count, s->count);
	if (c == 0) {
		return 0;
	}

	if (crb_trunk_init(d, s->e_size, c) == -1) {
		return -1;
	}

	memcpy(d->data, s->data, c * s->e_size);
	d->count = c;

	return c;
}

int crb_trunk_min_copy(void *dest, void *src)
{
	crb_assert(dest != NULL && src != NULL, return -1);

	return crb_trunk_copy(dest, src, CRB_TRUNK(src)->count);
}

