.PHONY: all build test clean
all: clean build test

build:
	rm -rf build
	mkdir build
	flex -o build/lex.yy.c src/lexer.lex
	bison -t -v -d -o build/parser.tab.c src/grammar.y
	g++ -g -o build/parser build/lex.yy.c build/parser.tab.c src/ASTNodes.cpp src/SymbolTable.cpp -lfl -std=c++14

test:
	./tests/run_tests.sh

clean:
	rm -rf build/*
graph:
	hpp2plantuml -i include/ASTNodes.h | sudo docker run -i ghcr.io/plantuml/plantuml -p  > graph.png