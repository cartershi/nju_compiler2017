parser: syntax.tab.c tree.c main.c semantic.c
	gcc syntax.tab.c tree.c semantic.c main.c -lfl -ly -o parser
lex.yy.c: lexical.l
	flex lexical.l
syntax.tab.c: syntax.y lex.yy.c
	bison -d -v syntax.y
clean:
	rm lex.yy.c syntax.tab.c syntax.tab.h parser
