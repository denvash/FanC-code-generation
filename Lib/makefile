all: clean
	flex scanner.lex
	bison -d parser.ypp
	g++ -std=c++11 -o parser *.c *.cpp
clean:
	rm -f lex.yy.c
	rm -f parser.tab.*pp
	rm -f parser