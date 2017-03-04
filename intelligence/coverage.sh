#!/bin/sh

rm -rf build/coverage
mkdir -p build/coverage
cp src/* build/coverage/.
cp tests/*.c build/coverage/.

cd build/coverage

clang -Ofast  -o coverageTestSuite *.c -g -lm -fsanitize=address -fno-omit-frame-pointer -fprofile-arcs -ftest-coverage -I ../../../criterion/include/ ../../../criterion/lib/libcriterion.so || exit 1

LD_LIBRARY_PATH=`pwd`/../../../criterion/lib/ ./coverageTestSuite || exit 1

rm *Test.c
echo "################" >> coverage
echo "##  Coverage  ##" >> coverage
echo "################" >> coverage
llvm-cov gcov *.c >> coverage
cat coverage

cd ../..

exit 0
