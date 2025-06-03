.PHONY: all build test clean
all: clean build test

build:
	rm -rf build
	mkdir build
	g++ -g -o build/parser src/main.cpp src/lexer.cpp src/parser.cpp src/ASTNodes.cpp src/SymbolTable.cpp src/SemanticAnalyzer.cpp -std=c++14
test:
	./tests/run_tests.sh
clean:
	rm -rf build/*
graph:
	hpp2plantuml -i include/ASTNodes.h | sudo docker run -i ghcr.io/plantuml/plantuml -p  > graph.png