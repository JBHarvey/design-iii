#!/bin/sh

clang -Ofast  -o client client.c protocol_callbacks.c ../../shared/{network.c,protocol.c,Communication.c} -I ../../shared/ -g -lm -fsanitize=address -fno-omit-frame-pointer -I ../src/ -lev
clang -Ofast  -o server server.c protocol_callbacks.c ../../shared/{network.c,protocol.c,Communication.c} -I ../../shared/ -g -lm -fsanitize=address -fno-omit-frame-pointer -I ../src/ -lev
clang -Ofast  -o test test.c ../../shared/{network.c,Communication.c} -I ../../shared/ -g -lm -fsanitize=address -fno-omit-frame-pointer -I ../src/ -lev
