#ifndef CRB_TRUNK_H
#define CRB_TRUNK_H

#include <stdlib.h>

#define CRB_TRUNK_HEADER size_t e_size; unsigned int capacity, count; uint8_t *data

struct crb_trunk {
	CRB_TRUNK_HEADER;
};

extern int crb_trunk_init(void *trunk, size_t e_size, unsigned int capacity);
extern unsigned int crb_trunk_append(void *trunk, void *elements, unsigned int count);
extern int crb_trunk_copy(void *dest, void *src, int count);
extern int crb_trunk_min_copy(void *dest, void *src);

#endif // CRB_TRUNK_H
