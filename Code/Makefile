parser: syntax.tab.c tree.c main.c semantic.c operand.c assembly.c
	gcc -g syntax.tab.c tree.c semantic.c main.c operand.c assembly.c -lfl -ly -o parser
lex.yy.c: lexical.l
	flex lexical.l
syntax.tab.c: syntax.y lex.yy.c
	bison -d -v syntax.y
clean:
	rm lex.yy.c syntax.tab.c syntax.tab.h syntax.output parser
