#include <stdio.h>
#include "util/crb_util.h"
#include "crb_statement.h"

static char *type_desc[] = {
	"NONE", "EXP", "RETURN"
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
	case CRB_EXP_STATEMENT:
		crb_expression_free(&statement->u.exp_statement.expression);
		break;
	default:
		break;
	}

	free(statement);
	*pstatement = NULL;
}
