#ifndef CRB_UTIL_H
#define CRB_UTIL_H

#include <assert.h>

#ifdef CRB_DEBUG

#define crb_assert(_cond_, _handler_) do {\
	assert(_cond_);\
	if (0) {\
		_handler_;\
	}\
} while (0)

#else

#define crb_assert(_cond_, _handler_) do {\
	if (!(_cond_)) {\
		_handler_;\
	}\
}while (0)

#endif // CRB_DEBUG

#define crb_panic(_handler_) crb_assert(0, _handler_)

#endif // CRB_UTIL_H
