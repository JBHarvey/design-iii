#!/bin/sh

#[BUILD] Cleans everything except objects and dependency files
find build/coverage/ -type f ! \( -name "*.d" -o -name "*.o" \) -delete

#[SRC] Updates shared code
cp -u --preserve=all ../shared/CommunicationStructures.h src
cp -u --preserve=all ../shared/Defines.h src
cp -u --preserve=all ../vision/src/{vision.c,vision.h} src/

#[BUILD] Moves compilation and test files to build dir
cp -r src/* build/coverage/.
cp -r tests/* build/coverage/.

#[BUILD] Build markers.cpp
cp -u --preserve=all ../vision/src/markers.h build/coverage/
clang++ -c -o build/coverage/markers.o ../vision/src/markers.cpp -g -fno-omit-frame-pointer -I ../src/

#[BUILD] Builds test suite
cd build/coverage
rm station-main.c
glib-compile-resources --target=resources.c --generate-source station-resources/ui.gresource.xml
clang -O0 -o TestSuite `find . -maxdepth 1 -type f ! \( -name "*.h" -o -name "*resources.c" -o -name "*makefile" -o -name "*.json" \)` -g -lm -fno-omit-frame-pointer -fprofile-arcs -ftest-coverage -I ../../../criterion/include/ ../../../criterion/lib/libcriterion.so -lstdc++ -lm -lev `pkg-config --libs --cflags gtk+-3.0 opencv gmodule-2.0` || exit 1

#[BUILD] Runs tests and coverage check
LD_LIBRARY_PATH=`pwd`/../../../criterion/lib/ ./TestSuite || exit 1

#[BUILD] Prints coverage check results
rm *Test.c 
echo "################" >> coverage
echo "##  Coverage  ##" >> coverage
echo "################" >> coverage
llvm-cov gcov *.c >> coverage
cat coverage

#[ROOT] Exits
cd ../..
exit 0
