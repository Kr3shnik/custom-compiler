.PHONY: all build test clean graph

# C++ compiler
CXX = g++
# C++ compiler flags
CXXFLAGS = -std=c++17 -Wall -g
# Linker flags
LDFLAGS =

# LLVM Configuration
LLVM_CXXFLAGS := $(shell llvm-config --cxxflags)
LLVM_LDFLAGS := $(shell llvm-config --ldflags --system-libs --libs core mcjit nativecodegen native all-targets)

# Update flags with LLVM
CXXFLAGS += $(LLVM_CXXFLAGS)
CXXFLAGS += -fexceptions
LDFLAGS += $(LLVM_LDFLAGS)

# Directories
SRCDIR = src
INCDIR = include
BUILDDIR = build

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
# Object files
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))
# Executable name
EXECUTABLE = $(BUILDDIR)/parser

all: $(EXECUTABLE)

# Rule to build the final executable
$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to build object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

test:
	-./tests/run_tests.sh all

test-parser:
	./tests/run_tests.sh parser

test-semantic:
	./tests/run_tests.sh semantic

clean:
	rm -rf $(BUILDDIR)

# Test target for code generation
test_codegen: all
	@mkdir -p $(BUILDDIR)/test
	# Step 1: Compile a test source file with our compiler to generate an object file
	./$(EXECUTABLE) tests/codegen/test1.c -o $(BUILDDIR)/test/test1.o
	# Step 2: Compile the C++ main/driver file
	clang++ -std=c++17 -c tests/codegen/main.cpp -o $(BUILDDIR)/test/main.o
	# Step 3: Link them together
	clang++ -std=c++17 $(BUILDDIR)/test/main.o $(BUILDDIR)/test/test1.o -o $(BUILDDIR)/test/a.out
	# Step 4: Run the executable
	@echo "Running linked executable:"
	$(BUILDDIR)/test/a.out

graph:
	hpp2plantuml -i include/ASTNodes.h | sudo docker run -i ghcr.io/plantuml/plantuml -p  > graph.png