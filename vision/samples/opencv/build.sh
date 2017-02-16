clang -Ofast -o jpg_to_figure jpg_to_figure.c ../../src/vision.c -g -lopencv_core -lm -lopencv_imgproc -lopencv_highgui -fsanitize=address -fno-omit-frame-pointer -I ../../src/ 
clang -Ofast -o webcam webcam_vision.c ../../src/vision.c -g -lopencv_core -lm -lopencv_imgproc -lopencv_highgui -fsanitize=address -fno-omit-frame-pointer -I ../../src/ 
