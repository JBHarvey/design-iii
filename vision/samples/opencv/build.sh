clang -Ofast -o jpg_to_figure jpg_to_figure.c ../../src/vision.c -g `pkg-config --libs opencv` -lm  -fsanitize=address -fno-omit-frame-pointer -I ../../src/ 
clang -Ofast -o webcam webcam_vision.c ../../src/vision.c -g `pkg-config --libs opencv` -lm -fsanitize=address -fno-omit-frame-pointer -I ../../src/ 
