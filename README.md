# design-iii

## Vision

Dependencies: opencv clang  
How to run tests: 
```
cd vision
./runtest.sh
```

## Base station

Dependencies: opencv clang gtk+-3.0 gmodule-2.0

The base station is built on GTK+3.0 with OpenCV. In order to make it works,  
you must recompile OpenCV with those three flags:  
`-DENABLE_PRECOMPILED_HEADERS=OFF`  
`-DWITH_GTK=ON`  
`-DWITH_GTK3=ON`

How to build and run:
```
cd ui
make all
./station-main
```


## Intelligence

Dependency: [docker](https://docs.docker.com/engine/getstarted/)  
How to run tests:
```
cd intelligence
./build-docker.sh
./runtest.sh
```
To see the test log in details:
```
./showlog.sh
```
