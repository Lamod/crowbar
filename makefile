TARGET=crowbar
CC=clang
OBJS=\
     util/crb_trunk.o \
     crb_type.o \
     crb_expression.o \
     crb_statement.o \
     crb_eval_exp.o \
     crb_exec.o \
     crb_interpreter.o \
     crowbar.tab.o \
     lex.yy.o \
     main.o
INCLUDES=\
	 crb_expression.h \
	 crb_eval_exp.h \
	 crb_exec.h \
	 crb_type.h \
	 crb_statement.h \
	 crb_interpreter.h \
	 util/crb_trunk.h \
	 util/crb_util.h

$(TARGET):$(OBJS)
	$(CC) $(OBJS) -g -o $(TARGET)
clean:
	rm util/*.o *.o lex.yy.c crowbar.tab.h crowbar.tab.c crowbar.output crowbar

lex.yy.o: crowbar.l
	flex crowbar.l
	$(CC) -c -o lex.yy.o lex.yy.c
crowbar.tab.o: crowbar.y
	bison -dv crowbar.y
	$(CC) -c -o crowbar.tab.o crowbar.tab.c

util/crb_trunk.o: util/crb_trunk.c $(INCLUDES)
crb_type.o: crb_type.c $(INCLUDES)
crb_expression.o: crb_expression.c $(INCLUDES)
crb_statement.o: crb_statement.c $(INCLUDES)
crb_interpreter.o: crb_interpreter.c $(INCLUDES)
crb_eval_exp.o: crb_eval_exp.c $(INCLUDES)
crb_exec.o: crb_exec.c $(INCLUDES)
main.o: main.c $(INCLUDES)
