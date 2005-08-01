
all: gen_srberror ../src/include/srb_error.h 

clean:
	rm -f y.tab.c lex.yy.c y.tab.o lex.yy.o gen_srberror gen_srberror.o y.tab.h
	

gen_srberror: y.tab.c lex.yy.c
	cc -g -o gen_srberror y.tab.c lex.yy.c -ly -ll

y.tab.c: err.p
	yacc -d err.p

lex.yy.c: err.l
	lex err.l
../src/include/srb_error.h: gen_srberror input/errDesc.in
	./gen_srberror input/errDesc.in > ../src/include/srb_error.h
