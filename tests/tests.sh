 clang -Ofast  -o StateTest StateTest.c ../src/State.c -g -lopencv_core -lm -lopencv_imgproc -lopencv_highgui -fsanitize=address -fno-omit-frame-pointer -I ../src/ -I ../criterion/include/ ../criterion/lib/libcriterion.so
 
LD_LIBRARY_PATH=`pwd`/../criterion/lib/ ./StateTest
