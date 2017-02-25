#!/bin/bash

suceeded_tests=0
failed_tests=0

vision_test=$(readlink -f "vision_test")
camera_calibration=$(readlink -f "camera_calibration.xml")

for i in vision_test_images/* ; do
  for j in $i/*.jpg; do
    if [[ " ${SUCCEEDED_IMAGES[*]} " == *" $i$j "* ]]
    then
        #echo test already $i $j
        suceeded_tests=$((suceeded_tests + 1))
    else
        if $vision_test $j $i/contours.xml $camera_calibration
        then
            #echo test suceeded $i $j
            SUCCEEDED_IMAGES+=($i$j)
            suceeded_tests=$((suceeded_tests + 1))
        else
            echo test failed $i $j
            failed_tests=$((failed_tests + 1))
        fi
    fi
  done
done

echo suceeded: $suceeded_tests
echo failed: $failed_tests

total_tests=$(($suceeded_tests + $failed_tests))
percent_succeed=$(echo "scale=2; $suceeded_tests*100/$total_tests" | bc)

echo Percent succeeded: $percent_succeed %
echo

if [ "$failed_tests" -eq "0" ]; then
   return 0;
fi

return 1
