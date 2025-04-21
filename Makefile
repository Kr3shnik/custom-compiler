.PHONY: build test clean
build:
	flex -o build/lex.yy.c src/lexer.lex
	bison -t -v -d -o build/parser.tab.c src/parser.y
	g++ -o build/parser build/lex.yy.c build/parser.tab.c -lfl
test:
	build/parser < tests/test2
clean:
	rm -rf build/*