#include <stdio.h>
#include "util/crb_stack.h"
#include "crb_type.h"
#include "crb_expression.h"
#include "crb_interpreter.h"
#include "crowbar.tab.h"

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

	printf("----------------------------\n");
	printf("parsing finished, start executing\n");
	printf("----------------------------\n");

	crb_interpreter_run(itp);

	crb_interpreter_free(&itp);

	return 0;
}
