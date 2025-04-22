.PHONY: build test clean
all: build test
build:
	flex -o build/lex.yy.c src/lexer.lex
	bison -t -v -d -o build/parser.tab.c src/parser.y
	g++ -o build/parser build/lex.yy.c build/parser.tab.c -lfl
test:
	./tests/run_tests.sh
clean:
	rm -rf build/*