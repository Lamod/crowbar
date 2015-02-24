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

#define crb_trunk_get_element(_t_, _type_, _idx_)\
	((_type_)(_t_)->data)[_idx_]

#define crb_trunk_read_element(_t_, _var_, _idx_)\
	(*(_var_) = crb_trunk_get_element(_t_, typeof(_var_), _idx_))

#define crb_trunk_return_if_empty(_t_) do{\
	if ((_t_)->count == 0) {\
		return;\
	}\
} while(0)

#define crb_trunk_return_exp_if_empty(_t_, _e_) do{\
	if ((_t_)->count == 0) {\
		return (_e_);\
	}\
} while(0)

#endif // CRB_TRUNK_H
