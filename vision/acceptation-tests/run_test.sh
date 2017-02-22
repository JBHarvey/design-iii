#!/bin/bash
SUCCEEDED_IMAGES=()

TOLERANCE=83.0 ./build.sh || exit 1

echo execute with tolerance 1000/83 +-0.36cm
. ./execute_test.sh

TOLERANCE=71.0 ./build.sh || exit 1

echo execute with tolerance 1000/71 +-0.42cm
. ./execute_test.sh

TOLERANCE=66.0 ./build.sh || exit 1

echo execute with tolerance 1000/66 +-0.45cm
. ./execute_test.sh

TOLERANCE=58.0 ./build.sh || exit 1

echo execute with tolerance 1000/58 +-0.51cm
. ./execute_test.sh

TOLERANCE=50.0 ./build.sh || exit 1

echo execute with tolerance 1000/50 +-0.6cm
. ./execute_test.sh

TOLERANCE=45.0 ./build.sh || exit 1

echo execute with tolerance 1000/45 +-0.67cm
. ./execute_test.sh || exit 1

exit 0
