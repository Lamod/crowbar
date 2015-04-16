#include <stdio.h>
#include "util/crb_util.h"
#include "crb_statement.h"

static char *type_desc[] = {
	"NONE", "EXP", "IF", "FOR", "CONTINUE", "BREAK", "RETURN"
};

struct crb_statement *crb_create_statement(int type, void *value)
{
	crb_assert(type <= CRB_RETURN_STATEMENT, return NULL);

	struct crb_statement *s = malloc(sizeof(struct crb_statement));
	if (s == NULL) {
		return NULL;
	}

	s->type = type;

	printf("%s t:%s(%d) v:%p\n", __func__, type_desc[type], type, value);

#define V(_t_) (*((_t_ *)value))
#define SETV(_f_) (s->u._f_ = V(typeof(s->u._f_)))

	switch(type) {
	case CRB_EXP_STATEMENT:
		SETV(exp_statement);
		break;
	case CRB_IF_STATEMENT:
		SETV(if_statement);
		break;
	case CRB_FOR_STATEMENT:
		SETV(for_statement);
		break;
	case CRB_CONTINUE_STATEMENT:
		break;
	case CRB_BREAK_STATEMENT:
		break;
	case CRB_RETURN_STATEMENT:
		SETV(return_statement);
		break;
	default:
		assert(0);
		break;
	}

#undef V
#undef SETV

	return s;
}

void crb_statement_free(struct crb_statement **pstatement)
{
	if (pstatement == NULL) {
		return;
	}

	struct crb_statement *statement = *pstatement;
	if (statement == NULL) {
		return;
	}

	switch (statement->type) {
	case CRB_RETURN_STATEMENT:
		crb_expression_free(&statement->u.return_statement.expression);
		break;
	case CRB_IF_STATEMENT:
	{
		struct crb_if_statement *if_statement = &statement->u.if_statement;

		crb_expression_free(&if_statement->condition);

		struct crb_statement *statement = NULL;
		for (int i = if_statement->main_block.statements.count - 1; i >= 0; --i) {
			crb_stack_read_element(&if_statement->main_block.statements, &statement, i);
			crb_statement_free(&statement);
		}
		crb_stack_destroy(&if_statement->main_block.statements);

		switch (if_statement->else_branch.type) {
		case CRB_ELSE_BRANCH:
		{
			struct crb_stack *statements = &if_statement->else_branch.u.else_block.statements;
			struct crb_statement *statement = NULL;
			for (int i = statements->count - 1; i >= 0; --i) {
				crb_stack_read_element(statements, &statement, i);
				crb_statement_free(&statement);
			}
			crb_stack_destroy(statements);
		}
			break;
		case CRB_ELSE_IF_BRANCH:
			crb_statement_free(&if_statement->else_branch.u.else_if_statement);
			break;
		default:
			break;
		}
	}
		break;
	case CRB_FOR_STATEMENT:
	{
		struct crb_for_statement *for_statement = &statement->u.for_statement;

		crb_expression_free(&for_statement->init);
		crb_expression_free(&for_statement->condition);
		crb_expression_free(&for_statement->post);

		struct crb_statement *statement = NULL;
		for (int i = for_statement->block.statements.count - 1; i >= 0; --i) {
			crb_stack_read_element(&for_statement->block.statements, &statement, i);
			crb_statement_free(&statement);
		}
		crb_stack_destroy(&for_statement->block.statements);
	}
		break;
	case CRB_EXP_STATEMENT:
		crb_expression_free(&statement->u.exp_statement.expression);
		break;
	default:
		break;
	}

	free(statement);
	*pstatement = NULL;
}
