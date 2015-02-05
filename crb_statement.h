#ifndef CRB_STATEMENT_H
#define CRB_STATEMENT_H

#include "crb_expression.h"

enum {
	CRB_STATEMENT_TYPE_NONE,
	CRB_EXP_STATEMENT,
};

struct crb_exp_statement {
	struct crb_expression *expression;
};

struct crb_statement {
	int type;
	union {
		struct crb_exp_statement exp_statement;
	} u;
};

extern struct crb_statement *crb_create_statement(int type, void *value);

#define crb_create_exp_statement(_e_) ({\
	struct crb_exp_statement s = { .expression = (_e_) };\
	crb_create_statement(CRB_EXP_STATEMENT, &s);\
})

#endif //CRB_STATEMENT_H