#!/bin/sh
fail=0

#### Intelligence
echo "----------     Running Intelligence module tests      ----------"
cd intelligence
./runtest.sh

cd ..

#### Vision
echo "----------     Running Vision module tests      ----------"
cd vision
./runtest.sh || $fail=1

cd ..

#### UI

# ?


exit $fail
