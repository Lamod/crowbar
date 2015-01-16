%{
#include <stdio.h>
#include <string.h>
#include "crb_string.h"

static struct crb_string str;

%}

%x STRING

INTEGER [1-9][0-9]*
FLOAT (0?|{INTEGER})\.[0-9]+

%%

<INITIAL>^{INTEGER}$ {
	int v = 0;
	sscanf(yytext, "%d", &v);
	printf("integer: %d\n", v);
}

<INITIAL>^{FLOAT}$ {
	double v = 0;
	sscanf(yytext, "%lf", &v);
	printf("float: %lf\n", v);
}

<INITIAL>^[_a-zA-Z][_a-zA-Z0-9]*$ {
	printf("identifer: %s\n", yytext);
}

<INITIAL>\n {
}

<INITIAL>\" {
	if (crb_string_init(&str, 1024) != 0) {
		perror("crb_string_init");
		exit(1);
	}
	BEGIN(STRING);
}

<INITIAL>. {
	printf("%s\n", yytext);
}

<STRING>\" {
	printf("string: %s\n", str.data);
	free(str.data);
	memset(&str, 0, sizeof(str));
	BEGIN(INITIAL);
}

<STRING>"\\\\" {
	crb_string_append_char(&str, '\\');
}

<STRING>"\\\"" {
	crb_string_append_char(&str, '\"');
}

<STRING>"\\n" {
	crb_string_append_char(&str, '\n');
}

<STRING>"\\t" {
	crb_string_append_char(&str, '\t');
}

<STRING>"\\b" {
	crb_string_append_char(&str, '\b');
}

<STRING>. {
	crb_string_append_char(&str, yytext[0]);
}

