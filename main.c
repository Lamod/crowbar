#include <stdio.h>
#include "y.tab.h"
#include "crb_type.h"
#include "crb_expression.h"
#include "crb_interpreter.h"

extern FILE *yyin;
extern int yyparse();

struct crb_interpreter *itp = NULL;

int main(int argc, char *argv[])
{
	FILE *fp = stdin;
	if (argc > 1) {
		fp = fopen(argv[1], "r");
	}

	yyin = fp;

	itp = crb_create_interpreter();

	yyparse();
	crb_interpreter_exec(itp);

	return 0;
}
