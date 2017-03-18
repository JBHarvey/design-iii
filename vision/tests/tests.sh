clang++ -c -o markers.o ../src/markers.cpp -g -fsanitize=address -fno-omit-frame-pointer -I ../src/ || exit 1
clang -Ofast  -o testsuite *.c ../src/*.c markers.o -g `pkg-config --libs opencv` -lm -fsanitize=address -fno-omit-frame-pointer -I ../src/ -I ../../criterion/include/ ../../criterion/lib/libcriterion.so -lstdc++ || exit 1
 
LD_LIBRARY_PATH=`pwd`/../../criterion/lib/ ./testsuite || exit 1

exit 0
