#!/bin/sh

#Cleans the build directory in preparation for the new build
cp autotools/* build/.

#Launches the build
docker run -v `pwd`/build:/build -t typhoon
