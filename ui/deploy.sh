#!/bin/sh

#[BUILD] Creates subdirs if not already there
mkdir -p build/deploy/obj
mkdir -p build/deploy/station-resources

#[BUILD] Cleans everything except objects and dependency files
find build/deploy/ -type f ! \( -name "*.d" -o -name "*.o" \) -delete

#[SRC] Updates shared code
cp -u --preserve=all ../shared/CommunicationStructures.h src
cp -u --preserve=all ../vision/src/{vision.c,vision.h} src/

#[BUILD] Moves all compilation files to build dir
find src -type f -regex '.*\.\(c\|\h\)' -exec cp -u --preserve=all \{\} build/deploy \;

#[AUTODEPLOY] Prepares dependencies list
echo "DEPS=" > autodeploy/DEPS
ls -lAh --color=never --format=single-column build/deploy | grep "\.h" --color=never >> autodeploy/DEPS
sed -i ':a;N;$!ba;s/\n/ /g' autodeploy/DEPS #Replaces all \n with ' '

#[AUTODEPLOY] Prepares objects list
echo "_OBJ=" > autodeploy/_OBJ
echo "station-main.o" >> autodeploy/_OBJ
ls -lAh --color=never --format=single-column build/deploy | grep "\.h" --color=never >> autodeploy/_OBJ
sed -i ':a;N;$!ba;s/\.h/.o/g' autodeploy/_OBJ #Replaces all .h with .o
sed -i ':a;N;$!ba;s/\n/ /g' autodeploy/_OBJ #Replaces all \n with ' '

#[AUTODEPLOY] Cleans former makefile
rm autodeploy/makefile

#[AUTODEPLOY] Generates new makefile
sed -e '/HFILES/{r autodeploy/DEPS' -e 'd}' autodeploy/templatefile >> autodeploy/tmpmakefile
sed -e '/OFILES/{r autodeploy/_OBJ' -e 'd}' autodeploy/tmpmakefile >> autodeploy/makefile

#[AUTODEPLOY] Cleans temporary files
rm autodeploy/tmpmakefile

#[BUILD] Moves the makefile to build dir
cp autodeploy/makefile build/deploy/makefile

#[BUILD] Moves other files to build dir
cp -r --preserve=all src/station-resources/ build/deploy/.
cp -r --preserve=all src/camera_calibration/ build/deploy/. 

#[BUILD] Copy Defines.h
cp -u --preserve=all ../shared/Defines.h build/deploy/

#[BUILD] Build markers.cpp
cp -u --preserve=all ../vision/src/markers.h build/deploy/
clang++ -c -o build/deploy/markers.o ../vision/src/markers.cpp -g -fsanitize=address -fno-omit-frame-pointer -I ../src/

#[BUILD] Launches the build
cd build/deploy
make all 

#[ROOT] Brings back the runnable and runs it
cd ../..
cp build/deploy/station-main .
./station-main
