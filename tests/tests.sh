 clang -Ofast  -o testsuite *.c ../src/*.c -g `pkg-config --libs opencv` -lm -fsanitize=address -fno-omit-frame-pointer -I ../src/ -I ../criterion/include/ ../criterion/lib/libcriterion.so
 
LD_LIBRARY_PATH=`pwd`/../criterion/lib/ ./testsuite
