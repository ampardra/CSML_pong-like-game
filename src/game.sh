#!/bin/bash

rm *.o *.out &>/dev/null
nasm -f elf64 update_ball.s -o update_ball.o
g++ main.cpp update_ball.o -o pong.out -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -no-pie
./pong.out
rm *.o *.out &>/dev/null