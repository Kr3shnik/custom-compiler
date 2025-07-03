.PHONY: all build test test_codegen clean

all: clean build test

build:
	rm -rf build
	mkdir build
	g++ -std=c++17 -Wall -g -I/usr/lib/llvm-14/include -std=c++14 -fno-exceptions -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -fexceptions -o build/parser src/lexer.cpp src/parser.cpp src/ASTNodes.cpp src/SymbolTable.cpp src/SemanticAnalyzer.cpp src/CodeGenerator.cpp src/main.cpp -L/usr/lib/llvm-14/lib -lLLVM-14

test:
	./tests/run_tests.sh

test_codegen:
	./tests/run_tests.sh codegen

clean:
	rm -rf build/*