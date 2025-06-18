#!/bin/bash

set -e

declare -i PASS=0
declare -i FAIL=0
TEST_TYPE=${1:-all} # Default to all tests if no argument is provided

run_parser_tests() {
    echo "🔎 Running PARSER SYNTACTIC FAILURE tests (should fail parsing)..."
    for file in tests/parser/failure/*; do
        if [ -f "$file" ]; then # Check if file exists to avoid errors with empty dirs
            if build/parser < "$file" > /dev/null 2>&1; then
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
            if build/parser < "$file" > /dev/null 2>&1; then
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
            if build/parser < "$file" > /dev/null 2>&1; then
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
            if build/parser < "$file" > /dev/null 2>&1; then
                echo "✅ PASS: $file (passed all checks)"
                PASS+=1
            else
                echo "❌ FAIL: $file (unexpectedly failed - check parser or semantic analyzer)"
                FAIL+=1
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
elif [ "$TEST_TYPE" == "all" ]; then
    echo "Running ALL tests (Parser and Semantic categories)..."
    run_parser_tests
    echo ""
    run_semantic_tests
else
    echo "❌ Invalid test type: $TEST_TYPE. Use 'parser', 'semantic', or 'all'."
    exit 1
fi

echo ""
echo "🧪 Test Summary:"
echo "✅ Passed: $PASS"
echo "❌ Failed: $FAIL"

# Exit with nonzero code if there were failures
exit $FAIL