# design-iii

## Base station

The base station is built on GTK+3.0 with OpenCV. In order to make it works,  
you must recompile OpenCV with those three flags:  
`-DENABLE_PRECOMPILED_HEADERS=OFF`  
`-DWITH_GTK=ON`  
`-DWITH_GTK3=ON`


