#!/bin/sh

rm -rf build/coverage
mkdir -p build/coverage

cp ../shared/CommunicationStructures.h src/.
cp ../shared/Defines.h src/.
cp -r src/* build/coverage/.
cp -r tests/* build/coverage/.

cd build/coverage

clang -O0 -o coverageTestSuite *.c -g `pkg-config --libs opencv` -lm -fsanitize=address -fno-omit-frame-pointer -fprofile-arcs -ftest-coverage -I ../../../criterion/include/ ../../../criterion/lib/libcriterion.so -lev || exit 1

LD_LIBRARY_PATH=`pwd`/../../../criterion/lib/ ./coverageTestSuite || exit 1

rm *Test.c
echo "################" >> coverage
echo "##  Coverage  ##" >> coverage
echo "################" >> coverage
llvm-cov gcov *.c >> coverage
cat coverage

cd ../..

exit 0
