clang -Ofast -o jpg_to_figure jpg_to_figure.c vision.c -g -lopencv_core -lm -lopencv_imgproc -lopencv_highgui -fsanitize=address -fno-omit-frame-pointer
clang -Ofast -o webcam webcam_vision.c vision.c -g -lopencv_core -lm -lopencv_imgproc -lopencv_highgui -fsanitize=address -fno-omit-frame-pointer
