%{

#include <stdio.h>
#include "crb_type.h"
#include "crb_expression.h"
#include "crb_statement.h"
#include "crb_eval_exp.h"
#include "crb_interpreter.h"
#include "util/crb_stack.h"

extern struct crb_interpreter *itp;

%}

%union {
	const char *identifier;

	struct crb_expression *expression;
	struct crb_statement *statement;

	struct crb_stack parameters;
	struct crb_stack statements;
	struct crb_stack arguments;
}

%token 	<expression> INTEGER_LITERAL FLOAT_LITERAL STRING_LITERAL
%token 	<identifier> IDENTIFIER
%token 	<expression> TRUE FALSE
%token 	ADD SUB MUL DIV MOD LP RP LC RC GT GE LT LE EQ NE
	LOGICAL_AND LOGICAL_OR INVERT ASSIGN SEMICOLON COMMA
	FUNCTION RETURN IF ELSE
%type 	<expression> expression logical_or_expression logical_and_expression
	equality_expression relational_expression additive_expression
	multiplicative_expression unary_expression primary_expression
	function_defination
%type	<statement> statement function_defination_statement if_statement
%type	<statements> statement_list block
%type	<parameters> parameter_list
%type	<arguments> argument_list

%%

global_statement_list
	:statement_list
	{
		itp->statements = $1;
	}
	;
statement_list
	:statement
	{
		crb_stack_init(&$$, sizeof($1), 1);
		crb_stack_append(&$$, &$1, 1);
	}
	|statement_list statement
	{ 
		crb_stack_append(&$1, &$2, 1);
		
		$$ = $1;
	}
	;
statement
	:expression SEMICOLON
	{
		$$ = crb_create_exp_statement($1);
	}
	| RETURN expression SEMICOLON
	{
		$$ = crb_create_return_statement($2);
	}
	| function_defination_statement
	| if_statement
	;
function_defination_statement
	:FUNCTION IDENTIFIER LP parameter_list RP block
	{
		struct crb_function f = { .parameters = $4, .statements = $6 };
		struct crb_expression *func_exp = crb_create_function_expression(f);
		struct crb_expression *assign_exp = crb_create_assign_expression($2, func_exp);
		$$ = crb_create_exp_statement(assign_exp);
	}
	;
if_statement
	: IF expression block
	{
		struct crb_if_statement if_statement = { .condition = $2, .main_statements = $3 };
		$$ = crb_create_statement(CRB_IF_STATEMENT, &if_statement);
	}
	| if_statement ELSE block
	{
		$1->u.if_statement.else_branch.type = CRB_ELSE_BRANCH;
		$1->u.if_statement.else_branch.u.else_statements = $3;

		$$ = $1;
	}
	| if_statement ELSE if_statement
	{
		$1->u.if_statement.else_branch.type = CRB_ELSE_IF_BRANCH;
		$1->u.if_statement.else_branch.u.else_if_statement = $3;

		$$ = $1;
	}
	;
expression
	: logical_or_expression
	| IDENTIFIER ASSIGN expression
	{
		$$ = crb_create_assign_expression($1, $3);
	}
	;
logical_or_expression
	: logical_and_expression
	| logical_or_expression LOGICAL_OR logical_and_expression
	{
		$$ = crb_create_binary_expression(CRB_BINARY_OPERATOR_LOGICAL_OR, $1, $3);
	}
	;
logical_and_expression
	: equality_expression
	| logical_and_expression LOGICAL_AND equality_expression
	{
		$$ = crb_create_binary_expression(CRB_BINARY_OPERATOR_LOGICAL_AND, $1, $3);
	}
	;
equality_expression
	: relational_expression
	| equality_expression EQ relational_expression
	{
		$$ = crb_create_binary_expression(CRB_BINARY_OPERATOR_EQ, $1, $3);
	}
	| equality_expression NE relational_expression
	{
		$$ = crb_create_binary_expression(CRB_BINARY_OPERATOR_NE, $1, $3);
	}
	;
relational_expression
	: additive_expression
	| relational_expression GT additive_expression
	{
		$$ = crb_create_binary_expression(CRB_BINARY_OPERATOR_GT, $1, $3);
	}
	| relational_expression GE additive_expression
	{
		$$ = crb_create_binary_expression(CRB_BINARY_OPERATOR_GE, $1, $3);
	}
	| relational_expression LT additive_expression
	{
		$$ = crb_create_binary_expression(CRB_BINARY_OPERATOR_LT, $1, $3);
	}
	| relational_expression LE additive_expression
	{
		$$ = crb_create_binary_expression(CRB_BINARY_OPERATOR_LE, $1, $3);
	}
	;
additive_expression
	: multiplicative_expression
	| additive_expression ADD multiplicative_expression
	{
		$$ = crb_create_binary_expression(CRB_BINARY_OPERATOR_ADD, $1, $3);
	}
	| additive_expression SUB multiplicative_expression
	{
		$$ = crb_create_binary_expression(CRB_BINARY_OPERATOR_SUB, $1, $3);
	}
	;
multiplicative_expression
	: unary_expression
	| multiplicative_expression MUL unary_expression
	{
		$$ = crb_create_binary_expression(CRB_BINARY_OPERATOR_MUL, $1, $3);
	}
	| multiplicative_expression DIV unary_expression
	{
		$$ = crb_create_binary_expression(CRB_BINARY_OPERATOR_DIV, $1, $3);
	}
	| multiplicative_expression MOD unary_expression
	{
		$$ = crb_create_binary_expression(CRB_BINARY_OPERATOR_MOD, $1, $3);
	}
	;
unary_expression
	: primary_expression
	| SUB unary_expression
	{
		$$ = crb_create_unary_expression(CRB_UNARY_OPERATOR_MINUS, $2);
	}
	| INVERT unary_expression
	{
		$$ = crb_create_unary_expression(CRB_UNARY_OPERATOR_INVERT, $2);
	}
	;
primary_expression
	: LP expression RP
	{
		$$ = $2;
	}
	| INTEGER_LITERAL
	| FLOAT_LITERAL
	| STRING_LITERAL
	| IDENTIFIER
	{
		$$ = crb_create_identifier_expression($1);
	}
	| TRUE
	| FALSE
	| IDENTIFIER LP argument_list RP
	{
		$$ = crb_create_function_call_expression($1, &$3);
	}
	| IDENTIFIER LP RP
	{
		$$ = crb_create_function_call_expression($1, NULL);
	}
	| function_defination
	;
argument_list
	:expression
	{
		crb_stack_init(&$$, sizeof($1), 1);
		crb_stack_append(&$$, &$1, 1);
	}
	|argument_list COMMA expression
	{
		crb_stack_append(&$1, &$3, 1);
		$$ = $1;
	}
	;
function_defination
	:FUNCTION LP parameter_list RP block
	{
		struct crb_function f = { .parameters = $3, .statements = $5 };
		$$ = crb_create_function_expression(f);
	}
	;
parameter_list
	:IDENTIFIER
	{
		crb_stack_init(&$$, sizeof($1), 1);
		crb_stack_append(&$$, &$1, 1);
	}
	|parameter_list COMMA IDENTIFIER
	{
		crb_stack_append(&$1, &$3, 1);

		$$ = $1;
	}
	;
block
	: LC RC
	{
		$$ = (struct crb_stack){0};
	}
	| LC statement_list RC
	{
		$$ = $2;
	}
	;
%%

int yyerror (char const *s)
{
	fprintf(stderr, "%s <%p>:%s\n", __func__, s, s);
	return 0;
}

