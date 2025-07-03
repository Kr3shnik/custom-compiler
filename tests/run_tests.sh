#!/bin/bash

set -e

declare -i PASS=0
declare -i FAIL=0
TEST_TYPE=${1:-all} # Default to all tests if no argument is provided

run_parser_tests() {
    echo "🔎 Running PARSER SYNTACTIC FAILURE tests (should fail parsing)..."
    for file in tests/parser/failure/*; do
        if [ -f "$file" ]; then # Check if file exists to avoid errors with empty dirs
            if build/parser "$file" > /dev/null 2>&1; then
                echo "❌ FAIL: $file (unexpectedly passed parsing)"
                FAIL+=1
            else
                echo "✅ PASS: $file (correctly failed parsing)"
                PASS+=1
            fi
        fi
    done

    echo ""
    echo "🔎 Running PARSER SYNTACTIC SUCCESS tests (should pass parsing)..."
    for file in tests/parser/success/*; do
        if [ -f "$file" ]; then # Check if file exists
            # For these tests, we only care if the parser accepts the syntax.
            # Semantic correctness is checked in the semantic tests.
            if build/parser "$file" > /dev/null 2>&1; then
                echo "✅ PASS: $file (passed parsing)"
                PASS+=1
            else
                echo "❌ FAIL: $file (unexpectedly failed parsing)"
                FAIL+=1
            fi
        fi
    done
}

run_semantic_tests() {
    echo "🔎 Running SEMANTIC FAILURE tests (should pass parsing, fail semantics)..."
    for file in tests/semantic/failure/*; do
        if [ -f "$file" ]; then # Check if file exists
            # We expect these to fail, indicating a semantic error was caught.
            if build/parser "$file" > /dev/null 2>&1; then
                echo "❌ FAIL: $file (unexpectedly passed semantic checks)"
                FAIL+=1
            else
                echo "✅ PASS: $file (correctly failed semantic checks)"
                PASS+=1
            fi
        fi
    done

    echo ""
    echo "🔎 Running SEMANTIC SUCCESS tests (should pass all checks: parsing and semantics)..."
    for file in tests/semantic/success/*; do
        if [ -f "$file" ]; then # Check if file exists
            if build/parser "$file" > /dev/null 2>&1; then
                echo "✅ PASS: $file (passed all checks)"
                PASS+=1
            else
                echo "❌ FAIL: $file (unexpectedly failed - check parser or semantic analyzer)"
                FAIL+=1
            fi
        fi
    done
}

run_codegen_tests() {
    echo "🔎 Running CODE GENERATION SUCCESS tests (should compile and run correctly)..."
    
    # Create test build directory
    mkdir -p build/test_runner
    
    for file in tests/codegen/success/*.c; do
        if [ -f "$file" ]; then
            base_name=$(basename "$file" .c)
            main_file="tests/codegen/success/${base_name}_main.cpp"
            
            if [ -f "$main_file" ]; then
                echo "Testing $base_name..."
                
                # Step 1: Compile with our compiler
                if build/parser "$file" -o "build/test_runner/${base_name}.o" > /dev/null 2>&1; then
                    # Step 2: Compile the C++ main file
                    if clang++ -std=c++17 -c "$main_file" -o "build/test_runner/${base_name}_main.o" > /dev/null 2>&1; then
                        # Step 3: Link them together
                        if clang++ -std=c++17 "build/test_runner/${base_name}_main.o" "build/test_runner/${base_name}.o" -o "build/test_runner/${base_name}" > /dev/null 2>&1; then
                            # Step 4: Run the executable
                            if "build/test_runner/${base_name}" > /dev/null 2>&1; then
                                echo "✅ PASS: $base_name (compiled and ran successfully)"
                                PASS+=1
                            else
                                echo "❌ FAIL: $base_name (compiled but failed at runtime)"
                                FAIL+=1
                            fi
                        else
                            echo "❌ FAIL: $base_name (linking failed)"
                            FAIL+=1
                        fi
                    else
                        echo "❌ FAIL: $base_name (C++ main compilation failed)"
                        FAIL+=1
                    fi
                else
                    echo "❌ FAIL: $base_name (compiler failed)"
                    FAIL+=1
                fi
            else
                echo "⚠️  SKIP: $base_name (no corresponding main file found)"
            fi
        fi
    done

    echo ""
    echo "🔎 Running CODE GENERATION FAILURE tests (should compile but may fail at runtime)..."
    
    for file in tests/codegen/failure/*.c; do
        if [ -f "$file" ]; then
            base_name=$(basename "$file" .c)
            main_file="tests/codegen/failure/${base_name}_main.cpp"
            
            if [ -f "$main_file" ]; then
                echo "Testing $base_name (expecting potential runtime issues)..."
                
                # Step 1: Compile with our compiler
                if build/parser "$file" -o "build/test_runner/${base_name}.o" > /dev/null 2>&1; then
                    # Step 2: Compile the C++ main file
                    if clang++ -std=c++17 -c "$main_file" -o "build/test_runner/${base_name}_main.o" > /dev/null 2>&1; then
                        # Step 3: Link them together
                        if clang++ -std=c++17 "build/test_runner/${base_name}_main.o" "build/test_runner/${base_name}.o" -o "build/test_runner/${base_name}" > /dev/null 2>&1; then
                            # Step 4: Run the executable (we expect this might fail)
                            timeout 5s "build/test_runner/${base_name}" > /dev/null 2>&1
                            exit_code=$?
                            if [ $exit_code -eq 0 ]; then
                                echo "⚠️  UNEXPECTED: $base_name (ran successfully, but was expected to have issues)"
                                PASS+=1
                            elif [ $exit_code -eq 124 ]; then
                                echo "✅ PASS: $base_name (timed out as expected)"
                                PASS+=1
                            else
                                echo "✅ PASS: $base_name (failed at runtime as expected)"
                                PASS+=1
                            fi
                        else
                            echo "❌ FAIL: $base_name (linking failed)"
                            FAIL+=1
                        fi
                    else
                        echo "❌ FAIL: $base_name (C++ main compilation failed)"
                        FAIL+=1
                    fi
                else
                    echo "❌ FAIL: $base_name (compiler failed)"
                    FAIL+=1
                fi
            else
                echo "⚠️  SKIP: $base_name (no corresponding main file found)"
            fi
        fi
    done
}

if [ "$TEST_TYPE" == "parser" ]; then
    echo "Running PARSER category tests..."
    run_parser_tests
elif [ "$TEST_TYPE" == "semantic" ]; then
    echo "Running SEMANTIC category tests..."
    run_semantic_tests
elif [ "$TEST_TYPE" == "codegen" ]; then
    echo "Running CODE GENERATION category tests..."
    run_codegen_tests
elif [ "$TEST_TYPE" == "all" ]; then
    echo "Running ALL tests (Parser, Semantic, and Code Generation categories)..."
    run_parser_tests
    echo ""
    run_semantic_tests
    echo ""
    run_codegen_tests
else
    echo "❌ Invalid test type: $TEST_TYPE. Use 'parser', 'semantic', 'codegen', or 'all'."
    exit 1
fi

echo ""
echo "🧪 Test Summary:"
echo "✅ Passed: $PASS"
echo "❌ Failed: $FAIL"

# Exit with nonzero code if there were failures
exit $FAIL