#! /bin/bash
export PATH=.:$PATH
./scanner ../test/example1.kpl | diff ../test/result1.txt -
./scanner ../test/example2.kpl | diff ../test/result2.txt -
./scanner ../test/example3.kpl | diff ../test/result3.txt -
./scanner ../test/example4.kpl | diff ../test/result4.txt -
./scanner ../test/example5.kpl | diff ../test/result5.txt -
./scanner ../test/example6.kpl | diff ../test/result6.txt -
./scanner ../test/test_bai2.kpl | diff ../test/result_bai2.txt - # Cái này em bổ sung test cho bài 2
