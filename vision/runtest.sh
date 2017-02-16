#!/bin/sh

ls
cd samples/opencv
sh ./build.sh
cd ../../
cd acceptation-tests/
./run_test.sh

