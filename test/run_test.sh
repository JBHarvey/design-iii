#!/bin/bash

clang -Ofast  -o vision_test vision_test.c ../src/vision.c -g -lopencv_core -lm -lopencv_imgproc -lopencv_highgui -fsanitize=address -fno-omit-frame-pointer -I ../src/ -DD_TOLERANCE=\(76.0\) || exit 1

echo execute with tolerance 1000/76 +-0.35cm
./execute_test.sh


clang -Ofast  -o vision_test vision_test.c ../src/vision.c -g -lopencv_core -lm -lopencv_imgproc -lopencv_highgui -fsanitize=address -fno-omit-frame-pointer -I ../src/ -DD_TOLERANCE=\(66.0\) || exit 1

echo execute with tolerance 1000/66 +-0.45cm
./execute_test.sh

clang -Ofast  -o vision_test vision_test.c ../src/vision.c -g -lopencv_core -lm -lopencv_imgproc -lopencv_highgui -fsanitize=address -fno-omit-frame-pointer -I ../src/ -DD_TOLERANCE=\(45.0\) || exit 1

echo execute with tolerance 1000/45 +-0.75cm
./execute_test.sh || exit 1

exit 0
