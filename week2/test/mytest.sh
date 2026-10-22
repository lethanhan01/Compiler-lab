#! /bin/bash
export PATH=.:$PATH

echo "===== RUNNING KPL PARSER TEST SUITE ====="
ALL_PASSED=1
for i in 1 2 3 4 5 6 7; do
    echo "--- Testing example$i.kpl ---"
    diff -u <(./parser ../test/example$i.kpl) ../test/result$i.txt
    if [ $? -eq 0 ]; then
        echo ">>> example$i.kpl: PASSED"
    else
        echo ">>> example$i.kpl: FAILED"
        ALL_PASSED=0
    fi
done

if [ $ALL_PASSED -eq 1 ]; then
    echo "========================================="
    echo "ALL 7 TEST CASES PASSED SUCCESSFULLY!"
    echo "========================================="
else
    echo "SOME TESTS FAILED."
fi
