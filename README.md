# design-iii

## Coding Standart
We use the following notations:
 - Structure :  `struct StructureName {};`
 - Enum  `enum EnumName {};`
 - Variable :  `struct StructureName structure_name;`  
 - Private Functions :  `void functionName(void);`  
 - Public Functions : `void Module_functionName(void);`
 - Constant :  `const type CONSTANT_NAME;`  

## Vision

Dependencies: opencv clang  
How to run tests: 
```
cd vision
./runtest.sh
```

## Base station

Dependencies: opencv clang gtk+-3.0 gmodule-2.0 libev

The base station is built on GTK+3.0 with OpenCV. In order to make it works,  
you must recompile OpenCV with those three flags:  
`-DENABLE_PRECOMPILED_HEADERS=OFF`  
`-DWITH_GTK=ON`  
`-DWITH_GTK3=ON`

How to build and run:
```
cd ui
./deploy.sh
cd ..
./station-main
```

How to run tests and coverage:
```
cd ui
./test_and_check_coverage.sh
```

## Robot

Dependency: opencv clang libev gsl 
How to run tests and see coverage:
```
cd robot
./coverage.sh
```
