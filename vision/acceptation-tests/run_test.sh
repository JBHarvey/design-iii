#!/bin/bash
SUCCEEDED_IMAGES=()
fail=0

TOLERANCE=199.0 ./build.sh || exit 1

echo execute with tolerance 1000/200 +-0.15cm
. ./execute_test.sh

TOLERANCE=142.0 ./build.sh || exit 1

echo execute with tolerance 1000/124 +-0.21cm
. ./execute_test.sh


TOLERANCE=111.0 ./build.sh || exit 1

echo execute with tolerance 1000/111 +-0.27cm
. ./execute_test.sh

TOLERANCE=90.0 ./build.sh || exit 1

echo execute with tolerance 1000/90 +-0.33cm
. ./execute_test.sh

TOLERANCE=76.0 ./build.sh || exit 1

echo execute with tolerance 1000/76 +-0.39cm
. ./execute_test.sh

TOLERANCE=66.0 ./build.sh || exit 1

echo execute with tolerance 1000/66 +-0.45cm
. ./execute_test.sh

TOLERANCE=58.0 ./build.sh || exit 1

echo execute with tolerance 1000/58 +-0.51cm
. ./execute_test.sh

TOLERANCE=52.0 ./build.sh || exit 1

echo execute with tolerance 1000/52 +-0.57cm
. ./execute_test.sh

TOLERANCE=47.0 ./build.sh || exit 1

echo execute with tolerance 1000/47 +-0.63cm
. ./execute_test.sh

TOLERANCE=43.0 ./build.sh || exit 1

echo execute with tolerance 1000/43 +-0.7cm
. ./execute_test.sh || fail=1

llvm-cov gcov ../src/vision.c -o ./

exit $fail
