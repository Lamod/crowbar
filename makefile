default:crowbar.l
	flex crowbar.l
	gcc -ll  crb_string.c lex.yy.c -o crowbar

crb_string_test:test/str/main.c crb_string.c crb_string.h
	gcc test/str/main.c crb_string.c -o crb_string_test
