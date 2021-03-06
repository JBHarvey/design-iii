clang++ -c -o markers.o ../../src/markers.cpp -fsanitize=address -fno-omit-frame-pointer -I ../../src/
clang -Ofast -o jpg_to_figure jpg_to_figure.c ../../src/vision.c -g `pkg-config --libs opencv` -lm  -fsanitize=address -fno-omit-frame-pointer -I ../../src/ 
clang -Ofast -o webcam webcam_vision.c ../../src/vision.c -g `pkg-config --libs opencv` -lm -fsanitize=address -fno-omit-frame-pointer -I ../../src/ 
clang -Ofast -o webcam_obstacles webcam_obstacles.c ../../src/vision.c markers.o -g `pkg-config --libs opencv` -lm -fsanitize=address -fno-omit-frame-pointer -I ../../src/ -lstdc++
clang -Ofast -o jpg_obstacles jpg_obstacles.c ../../src/vision.c -g `pkg-config --libs opencv` -lm -fsanitize=address -fno-omit-frame-pointer -I ../../src/
