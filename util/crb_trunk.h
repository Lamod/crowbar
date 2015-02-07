#ifndef CRB_TRUNK_H
#define CRB_TRUNK_H

#include <stdlib.h>

#define CRB_TRUNK_HEADER size_t e_size; unsigned int capacity, count; uint8_t *data

extern int crb_trunk_init(void *list, size_t e_size, unsigned int capacity);
extern unsigned int crb_trunk_append(void *list, void *elements, unsigned int count);

#endif // CRB_TRUNK_H
