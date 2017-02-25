clang -g -O0  -o vision_test vision_test.c ../src/vision.c -g `pkg-config --libs opencv` -lm -fno-omit-frame-pointer -I ../src/ -DD_TOLERANCE=\($TOLERANCE\) -fprofile-arcs -ftest-coverage || exit 1

exit 0
