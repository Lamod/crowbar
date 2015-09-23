#ifndef CRB_STACK_H
#define CRB_STACK_H

#include <stdlib.h>

typedef void(*crb_element_destroy_func)(void *element);
typedef void(*crb_element_enumerate_func)(void *stack,
		void *element, unsigned int index, int *stop);

#define CRB_STACK_HEADER size_t e_size;\
	unsigned int capacity, count;\
	void *data;\
	crb_element_destroy_func destroy_func;

struct crb_stack {
	CRB_STACK_HEADER;
};

extern int crb_stack_init(void *stack, size_t e_size, unsigned int capacity);
extern void crb_stack_destroy(void *stack);

extern unsigned int crb_stack_enumerate(void *stack,
		unsigned int count,
		crb_element_enumerate_func func);

extern unsigned int crb_stack_append(void *stack, void *elements, unsigned int count);
extern int crb_stack_copy(void *dest, void *src, int count);
extern int crb_stack_min_copy(void *dest, void *src);

#define crb_stack_get_element(_t_, _ptype_, _idx_)\
	((_ptype_)(_t_)->data)[_idx_]

#define crb_stack_read_element(_t_, _pvar_, _idx_)\
	(*(_pvar_) = crb_stack_get_element(_t_, typeof(_pvar_), _idx_))

#define crb_stack_return_if_empty(_t_) do{\
	if ((_t_)->count == 0) {\
		return;\
	}\
} while(0)

#define crb_stack_return_exp_if_empty(_t_, _e_) do{\
	if ((_t_)->count == 0) {\
		return (_e_);\
	}\
} while(0)

#endif // CRB_STACK_H
