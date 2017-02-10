#!/bin/sh

#Cleans the building directory
rm build/tests -r
cp tests build -r
#Prepares the base source selection
echo "criterion_tests_SOURCES =" > autotools/FilesToCompile
#Fetches the names of the files to compile and places them in a file
find tests -type f | grep .c --color=never >> autotools/FilesToCompile
#Remplaces all \n with ' '
sed -i ':a;N;$!ba;s/\n/ /g' autotools/FilesToCompile

rm autotools/Makefile.am
#Takes the formated file names and puts them in the Makefile
sed -e '/TODO/{r autotools/FilesToCompile' -e 'd}' autotools/Templatefile.am >> autotools/Makefile.am

#Cleans the build directory in preparation for the new build
cp autotools/* build/.

#Launches the build
docker run -v `pwd`/build:/build -t typhoon
