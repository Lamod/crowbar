%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crb_type.h"
#include "crb_expression.h"
#include "crowbar.tab.h"

static struct crb_string str;

int yywrap(void)
{
	return 1;
}

%}

%x STRING

INTEGER [1-9][0-9]*
FLOAT (0?|{INTEGER})\.[0-9]+

%%

<INITIAL>"("            return LP;
<INITIAL>")"            return RP;
<INITIAL>"{"            return LC;
<INITIAL>"}"            return RC;
<INITIAL>"&&"           return LOGICAL_AND;
<INITIAL>"||"           return LOGICAL_OR;
<INITIAL>"="            return ASSIGN;
<INITIAL>"=="           return EQ;
<INITIAL>"!"		return INVERT;
<INITIAL>"!="           return NE;
<INITIAL>">"            return GT;
<INITIAL>">="           return GE;
<INITIAL>"<"            return LT;
<INITIAL>"<="           return LE;
<INITIAL>"+"            return ADD;
<INITIAL>"-"            return SUB;
<INITIAL>"*"            return MUL;
<INITIAL>"/"            return DIV;
<INITIAL>"%"            return MOD;
<INITIAL>";"		return SEMICOLON;
<INITIAL>","		return COMMA;
<INITIAL>"function"	return FUNCTION;
<INITIAL>"return"	return RETURN;
<INITIAL>"if"		return IF;
<INITIAL>"else"		return ELSE;
<INITIAL>"for"		return FOR;
<INITIAL>"true" {
	yylval.expression = crb_create_boolean_expression(1);

	return TRUE;
}

<INITIAL>"false" {
	yylval.expression = crb_create_boolean_expression(0);

	return FALSE;
}

<INITIAL>[_a-zA-Z][_a-zA-Z0-9]* {
	yylval.identifier = strdup(yytext);

	return IDENTIFIER;
}

<INITIAL>{INTEGER}|"0" {
	yylval.expression = crb_create_int_expression(atoi(yytext));

	return INTEGER_LITERAL;
}

<INITIAL>{FLOAT} {
	yylval.expression = crb_create_double_expression(atof(yytext));

	return FLOAT_LITERAL;
}

<INITIAL>\" {
	if (crb_string_init(&str, 1024) != 0) {
		perror("crb_string_init");
		exit(1);
	}
	BEGIN(STRING);
}

<INITIAL>[ \t] {
}

<INITIAL>\n {
}

<INITIAL>. {
	printf("%s\n", yytext);
}

<STRING>\" {
	yylval.expression = crb_create_string_expression(str);

	BEGIN(INITIAL);

	return STRING_LITERAL;
}

<STRING>"\\\\" {
	if (crb_string_append_char(&str, '\\') <= 0) {
		exit(1);
	}
}

<STRING>"\\\"" {
	if (crb_string_append_char(&str, '\"') <= 0) {
		exit(1);
	}
}

<STRING>"\\n" {
	if (crb_string_append_char(&str, '\n') <= 0) {
		exit(1);
	}
}

<STRING>"\\t" {
	if (crb_string_append_char(&str, '\t') <= 0) {
		exit(1);
	}
}

<STRING>"\\b" {
	if (crb_string_append_char(&str, '\b') <= 0) {
		exit(1);
	}
}

<STRING>. {
	if (crb_string_append_char(&str, yytext[0]) <= 0) {
		printf("error crb_string_append_char\n");
		exit(1);
	}
}

