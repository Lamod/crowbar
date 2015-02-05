TARGET=crowbar
CC=clang
OBJS=\
     crb_type.o \
     crb_expression.o \
     crb_statement.o \
     crb_eval_exp.o \
     y.tab.o \
     lex.yy.o \
     main.o
INCLUDES=crb_expression.h crb_type.h crb_statement.h

$(TARGET):$(OBJS)
	$(CC) $(OBJS) -o $(TARGET)
clean:
	rm *.o lex.yy.c y.tab.h y.tab.c y.output crowbar

lex.yy.c: crowbar.l
	flex crowbar.l
y.tab.h: crowbar.y
	bison --yacc -dv crowbar.y
y.tab.c: crowbar.y
	bison --yacc -dv crowbar.y

y.tab.o: y.tab.c y.tab.h $(INCLUDES)
lex.yy.o: lex.yy.c $(INCLUDES)
crb_type.o: crb_type.c $(INCLUDES)
crb_expression.o: crb_expression.c $(INCLUDES)
crb_statement.o: crb_statement.c $(INCLUDES)
crb_eva_exp.o: crb_eval_exp.c $(INCLUDES)
main.o: main.c $(INCLUDES)
