%{

#include <stdio.h>
#include "crb_type.h"
#include "crb_expression.h"
#include "crb_statement.h"
#include "crb_eval_exp.h"
#include "crb_interpreter.h"
#include "util/crb_trunk.h"

extern struct crb_interpreter *itp;

%}

%union {
	const char *identifier;

	struct crb_expression *expression;
	struct crb_statement *statement;

	struct crb_trunk parameters;
	struct crb_trunk statements;
}

%token 	<expression> INTEGER_LITERAL FLOAT_LITERAL STRING_LITERAL
%token 	<identifier> IDENTIFIER
%token 	<expression> TRUE FALSE
%token 	ADD SUB MUL DIV MOD LP RP LC RC GT GE LT LE EQ NE
	LOGICAL_AND LOGICAL_OR INVERT ASSIGN SEMICOLON COMMA
	FUNCTION RETURN
%type 	<expression> expression logical_or_expression logical_and_expression
	equality_expression relational_expression additive_expression
	multiplicative_expression unary_expression primary_expression
	function_defination
%type	<statement> statement return_statement
%type	<statements> statement_list block
%type	<parameters> parameter_list

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
		struct crb_trunk t = {0};
		crb_trunk_init(&t, sizeof($1), 1);
		crb_trunk_append(&t, &$1, 1);

		$$ = t;
	}
	|statement_list statement
	{ 
		crb_trunk_append(&$1, &$2, 1);
		
		$$ = $1;
	}
	;
statement
	:expression SEMICOLON
	{
		$$ = crb_create_exp_statement($1);
	}
	| return_statement
	;
return_statement
	: RETURN expression SEMICOLON
	{
		$$ = crb_create_return_statement($2);
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
	| function_defination
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
		struct crb_trunk t = {0};
		crb_trunk_init(&t, sizeof($1), 1);
		crb_trunk_append(&t, &$1, 1);

		$$ = t;
	}
	|parameter_list COMMA IDENTIFIER
	{
		crb_trunk_append(&$1, &$3, 1);

		$$ = $1;
	}
	;
block
	: LC statement_list RC
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

