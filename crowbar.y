%{

#include <stdio.h>
#include "crb_type.h"
#include "crb_expression.h"
#include "crb_statement.h"
#include "crb_eval_exp.h"
#include "crb_interpreter.h"

extern struct crb_interpreter *itp;

%}

%union {
	const char *identifier;

	struct crb_expression *expression;
	struct crb_statement *statement;
}

%token 	<expression> INTEGER_LITERAL FLOAT_LITERAL STRING_LITERAL
%token 	<identifier> IDENTIFIER
%token 	<expression> TRUE FALSE
%token 	ADD SUB MUL DIV MOD LP RP GT GE LT LE EQ NE LOGICAL_AND LOGICAL_OR INVERT ASSIGN SEMICOLON
%type 	<expression> expression logical_or_expression logical_and_expression
	equality_expression relational_expression additive_expression
	multiplicative_expression unary_expression primary_expression
%type	<statement> statement

%%

statement_list
	:statement
	|statement_list statement
	;
statement
	:expression SEMICOLON
	{
		$$ = crb_create_exp_statement($1);
		crb_trunk_append(&(itp->statements), &$$, 1);
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
	;
%%

int yyerror (char const *s)
{
	fprintf(stderr, "%s <%p>:%s\n", __func__, s, s);
	return 0;
}

