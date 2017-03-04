#!/bin/sh

clang -Ofast  -o client client.c network.c protocol.c protocol_callbacks.c -g -lm -fsanitize=address -fno-omit-frame-pointer -I ../src/ -lev
clang -Ofast  -o server server.c network.c protocol.c protocol_callbacks.c -g -lm -fsanitize=address -fno-omit-frame-pointer -I ../src/ -lev
clang -Ofast  -o test test.c network.c -g -lm -fsanitize=address -fno-omit-frame-pointer -I ../src/ -lev
