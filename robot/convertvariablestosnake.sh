#!/bin/sh

find . -name '*.h' -exec sed -i -r 's/new_(\w)/new\U\1/g' "{}" \;
find . -name '*.c' -exec sed -i -r 's/new_(\w)/new\U\1/g' "{}" \;
find . -name '*.h' -exec sed -i -r ':loop; /.*\<([a-z]+([A-Z][a-z]+)+)\>.*/ { h; s//\1/; s/([A-Z])/_\l\1/g; G; s/(.*)\n(.*)\<[a-z]+([A-Z][a-z]+)+\>(.*)/\2\1\4/; b loop }' "{}" \;
find . -name '*.c' -exec sed -i -r ':loop; /.*\<([a-z]+([A-Z][a-z]+)+)\>.*/ { h; s//\1/; s/([A-Z])/_\l\1/g; G; s/(.*)\n(.*)\<[a-z]+([A-Z][a-z]+)+\>(.*)/\2\1\4/; b loop }' "{}" \;
