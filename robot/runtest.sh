#!/bin/sh

#Cleans the building directory
rm -rf build/code
mkdir -p build/code

#Updates shared code
cp ../shared/CommunicationStructures.h src/.
cp ../shared/Defines.h src/.

#Prepares compilation files
find src -type f -regex '.*\.\(c\|\h\)' -exec cp \{\} build \;
find src -type f -regex '.*\.\(c\|\h\)' -exec cp \{\} build/code \;
find tests -type f -regex '.*\.c' -exec cp \{\} build \;
find tests -type f -regex '.*\.c' -exec cp \{\} build/code \;

#Prepares the base source selection
echo "criterion_tests_SOURCES =" > autotools/FilesToCompile

#Fetches the names of the .c files to compile and places them in a file
ls -lAh --color=never --format=single-column build/code | grep "\.c" --color=never >> autotools/FilesToCompile

#Fetches the names of the .h files to compile and places them in a file
echo "criterion_tests_HEADERS =" > autotools/HeadersToCompile
ls -lAh --color=never --format=single-column build/code | grep "\.h" --color=never >> autotools/HeadersToCompile

#Remplaces all \n with ' '
sed -i ':a;N;$!ba;s/\n/ /g' autotools/FilesToCompile
sed -i ':a;N;$!ba;s/\n/ /g' autotools/HeadersToCompile

rm autotools/Makefile.am
#Takes the formated file names and puts them in the Makefile
sed -e '/CFILES/{r autotools/FilesToCompile' -e 'd}' autotools/Templatefile.am >> autotools/tmpMakefile.am
sed -e '/HFILES/{r autotools/HeadersToCompile' -e 'd}' autotools/tmpMakefile.am >> autotools/Makefile.am

rm autotools/tmpMakefile.am

#Cleans the build directory in preparation for the new build
cp autotools/* build/.

#Launches the build
docker run -v `pwd`/build:/build -t jbharvey/typhoon
