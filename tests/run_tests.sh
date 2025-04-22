#!/bin/bash

set -e

declare -i PASS=0
declare -i FAIL=0

echo "🔎 Running FAILURE tests (should fail)..."
for file in tests/failure/*; do
    echo $file
    if build/parser < "$file" > /dev/null 2>&1; then
        echo "❌ FAIL: $file (unexpected success)"
        FAIL+=1
    else
        echo "✅ PASS: $file (correctly failed)"
        PASS+=1
    fi
done

echo ""
echo "🔎 Running SUCCESS tests (should pass)..."
for file in tests/success/*; do
    if build/parser < "$file" > /dev/null 2>&1; then
        echo "✅ PASS: $file"
        PASS+=1
    else
        echo "❌ FAIL: $file (unexpected failure)"
        FAIL+=1
    fi
done

echo ""
echo "🧪 Test Summary:"
echo "✅ Passed: $PASS"
echo "❌ Failed: $FAIL"

# Exit with nonzero code if there were failures
exit $FAIL