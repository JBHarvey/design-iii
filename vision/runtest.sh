#!/bin/sh
fail=0

cd samples/opencv
sh ./build.sh
cd ../../
cd tests/
./tests.sh || fail=1
cd ..
cd acceptation-tests/
./run_test.sh || fail=1
cd ..

exit $fail
