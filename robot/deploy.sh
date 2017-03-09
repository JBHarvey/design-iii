#!/bin/sh

#Keep the previously built code
#If nothing has changed, a file won't be recompiled
rm -rf autodeploy/obj
mkdir autodeploy/obj
cp build/deploy/obj/* autodeploy/obj/.

#Cleans the building directory
rm -rf build/deploy
mkdir -p build/deploy
mkdir build/deploy/obj

#Updates shared code
cp ../shared/CommunicationStructures.h src/.

#Prepares compilation files
find src -type f -regex '.*\.\(c\|\h\)' -exec cp \{\} build/deploy \;
cp robot-main.c build/deploy/.

#Prepares the list of file to check for dependency
echo "DEPS=" > autodeploy/DEPS
ls -lAh --color=never --format=single-column build/deploy | grep "\.h" --color=never >> autodeploy/DEPS

#Prepares the list of file to build
echo "_OBJ=" > autodeploy/_OBJ
echo "robot-main.h" >> autodeploy/_OBJ
ls -lAh --color=never --format=single-column build/deploy | grep "\.h" --color=never >> autodeploy/_OBJ

#Remplaces all \n with ' '
sed -i ':a;N;$!ba;s/\n/ /g' autodeploy/DEPS

#Does the same but also replaces all the .h with .o
sed -i ':a;N;$!ba;s/\.h/.o/g' autodeploy/_OBJ
sed -i ':a;N;$!ba;s/\n/ /g' autodeploy/_OBJ

rm autodeploy/makefile

#Takes the formated file names and puts them in the Makefile
sed -e '/HFILES/{r autodeploy/DEPS' -e 'd}' autodeploy/templatefile >> autodeploy/tmpmakefile
sed -e '/OFILES/{r autodeploy/_OBJ' -e 'd}' autodeploy/tmpmakefile >> autodeploy/makefile

rm autodeploy/tmpmakefile

#Copies the finalized files for compilation in the build directory
cp autodeploy/makefile build/deploy/makefile
cp autodeploy/obj/* build/deploy/obj/.

#Launches the build
cd build/deploy
make 

#Brings back the runnable and runs it
cd ../..
cp build/deploy/robot-main .
./robot-main
