#include "crb_exec.h"
#include "crb_eval_exp.h"
#include "util/crb_util.h"
#include <stdio.h>
#include <assert.h>

enum {
	CRB_CONTROL_TYPE_NORMAL,
	CRB_CONTROL_TYPE_CONTINUE,
	CRB_CONTROL_TYPE_BREAK,
	CRB_CONTROL_TYPE_RETURN
};
	
struct crb_block_result {
	struct crb_value value;
	int control_type;
};

static struct crb_block_result exec_statement(
		struct crb_interpreter *itp,
		struct crb_statement *statement);
static struct crb_block_result exec_block(
		struct crb_interpreter *itp,
		const struct crb_block *block);

static struct crb_block_result exec_statement(
		struct crb_interpreter *itp,
		struct crb_statement *statement)
{
	printf("%s statement: %p %d\n", __func__, statement, statement->type);

	struct crb_block_result result = {0};

	switch (statement->type) {
	case CRB_EXP_STATEMENT:
		result.value = crb_eval_exp(itp, statement->u.exp_statement.expression);
		break;
	case CRB_IF_STATEMENT:
	{
		struct crb_if_statement *if_statement = &statement->u.if_statement;
		struct crb_value condition_result = crb_eval_exp(itp, if_statement->condition);
		if (!crb_is_boolean_value(condition_result)) {
			assert(0);
		}
		
		if (condition_result.u.boolean_value != 0) {
			result = exec_block(itp, &if_statement->main_block);
			break;
		}

		switch (if_statement->else_branch.type) {
		case CRB_ELSE_BRANCH:
			result = exec_block(itp, &if_statement->else_branch.u.else_block);
			break;
		case CRB_ELSE_IF_BRANCH:
			result = exec_statement(itp, if_statement->else_branch.u.else_if_statement);
			break;		
		default:
			break;
		}
	}
		break;
	case CRB_FOR_STATEMENT:
	{
		struct crb_for_statement *for_statement = &statement->u.for_statement;

		if (for_statement->init != NULL) {
			crb_eval_exp(itp, for_statement->init);
		}
		while (1) {
			if (for_statement->condition != NULL) {
				struct crb_value v = crb_eval_exp(itp, for_statement->condition); 
				if (!crb_is_boolean_value(v)) {
					assert(0);
				}

				if (v.u.boolean_value == 0) {
					break;
				}
			}

			result = exec_block(itp, &for_statement->block);
			if (result.control_type == CRB_CONTROL_TYPE_BREAK
				|| result.control_type == CRB_CONTROL_TYPE_RETURN) {

				break;
			}

			if (for_statement->post != NULL) {
				crb_eval_exp(itp, for_statement->post);
			}
		}
		if (result.control_type != CRB_CONTROL_TYPE_RETURN) {
			//handled by for-statement, reset to normal
			result.control_type = CRB_CONTROL_TYPE_NORMAL;
		}
	}
		break;
	case CRB_CONTINUE_STATEMENT:
		result.control_type = CRB_CONTROL_TYPE_CONTINUE;
		break;
	case CRB_BREAK_STATEMENT:
		result.control_type = CRB_CONTROL_TYPE_BREAK;
		break;
	case CRB_RETURN_STATEMENT:
		result.control_type = CRB_CONTROL_TYPE_RETURN;
		struct crb_expression *exp = statement->u.return_statement.expression;
		if (exp != NULL) {
			result.value = crb_eval_exp(itp, exp);
		}
		break;
	default:
		break;
	}

	return result;
}

static struct crb_block_result exec_block(
		struct crb_interpreter *itp,
		const struct crb_block *block)
{
	struct crb_block_result result = {0};
	struct crb_statement *statement = NULL;
	for (int i = 0; i < block->statements.count; ++i) {
		statement = ((struct crb_statement **)block->statements.data)[i];
		result = exec_statement(itp, statement);
		if (result.control_type != CRB_CONTROL_TYPE_NORMAL) {
			break;
		}
	}

	printf("%s %d ", __func__, block->type);
	crb_value_print(result.value);
	printf("\n");

	return result;
}

struct crb_value crb_exec_block(
		struct crb_interpreter *itp,
		const struct crb_block *block)
{
	crb_assert(itp != NULL && block != NULL && block->statements.count > 0,
			crb_do_nothing);

	return exec_block(itp, block).value;
}
