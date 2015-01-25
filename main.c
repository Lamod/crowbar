#include <stdio.h>
#include "y.tab.h"
#include "crb_type.h"
#include "crb_expression.h"

extern FILE *yyin;
extern int yyparse();

int main(int argc, char *argv[])
{
	FILE *fp = stdin;
	if (argc > 1) {
		fp = fopen(argv[1], "r");
	}

	yyin = fp;

	return yyparse();
}
