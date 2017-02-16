#!/bin/sh

#### Intelligence
echo "----------     Running Intelligence module tests      ----------"
cd intelligence
./runtest.sh

cd ..

#### Vision
echo "----------     Running Vision module tests      ----------"
cd vision
./runtest.sh

cd ..

#### UI

# ?
