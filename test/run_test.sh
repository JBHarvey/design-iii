#!/bin/bash

clang -Ofast  -o vision_test vision_test.c ../src/vision.c -g -lopencv_core -lm -lopencv_imgproc -lopencv_highgui -fsanitize=address -fno-omit-frame-pointer -I ../src/ || return 0

suceeded_tests=0
failed_tests=0

vision_test=$(readlink -f "vision_test")

for i in vision_test_images/* ; do
  cd $i
  for j in ./*.jpg; do
     if $vision_test $j ./contours.xml
     then
        echo test suceeded $i $j
        suceeded_tests=$((suceeded_tests + 1))
     else
        echo test failed $i $j
        failed_tests=$((failed_tests + 1))
     fi
  done
  cd ../..
done

echo suceeded: $suceeded_tests
echo failed: $failed_tests
