#ifndef CRB_STATEMENT_H
#define CRB_STATEMENT_H

#include "crb_type.h"
#include "crb_expression.h"

enum {
	CRB_STATEMENT_TYPE_NONE,
	CRB_EXP_STATEMENT,
	CRB_DEFINE_STATEMENT,
	CRB_IF_STATEMENT,
	CRB_FOR_STATEMENT,
	CRB_CONTINUE_STATEMENT,
	CRB_BREAK_STATEMENT,
	CRB_RETURN_STATEMENT,
};

struct crb_exp_statement {
	struct crb_expression *expression;
};

enum {
	CRB_ELSE_BRANCH_TYPE_NONE,
	CRB_ELSE_BRANCH,
	CRB_ELSE_IF_BRANCH
};

struct crb_define_statement {
	struct crb_stack expressions;
};

struct crb_if_statement {
	struct crb_expression *condition;
	struct crb_block main_block;

	struct {
		int type;
		union {
			struct crb_block else_block;
			struct crb_statement *else_if_statement;
		} u;
	} else_branch;
};

struct crb_for_statement {
	struct crb_expression *init, *condition, *post;

	struct crb_block block;
};

struct crb_return_statement {
	struct crb_expression *expression;
};

struct crb_statement {
	int type;
	union {
		struct crb_exp_statement exp_statement;
		struct crb_define_statement define_statement;
		struct crb_if_statement if_statement;
		struct crb_for_statement for_statement;
		struct crb_return_statement return_statement;
	} u;
};

extern struct crb_statement *crb_create_statement(int type, void *value);
extern void crb_statement_free(struct crb_statement **pstatement);

#define crb_create_exp_statement(_e_) ({\
	struct crb_exp_statement s = { .expression = (_e_) };\
	crb_create_statement(CRB_EXP_STATEMENT, &s);\
})

#define crb_create_define_statement(_e_) ({\
	struct crb_define_statement s = { .expressions = (_e_) };\
	crb_create_statement(CRB_DEFINE_STATEMENT, &s);\
})

#define crb_create_return_statement(_e_) ({\
	struct crb_return_statement s = { .expression = (_e_) };\
	crb_create_statement(CRB_RETURN_STATEMENT, &s);\
})

#endif //CRB_STATEMENT_H
