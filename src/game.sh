#!/bin/bash

rm *.o *.out &>/dev/null
nasm -f elf64 update_ball.s -o update_ball.o
nasm -f elf64 update_ball_movement.s -o update_ball_movement.o
nasm -f elf64 update_mode_controls.s -o update_ball_controls.o
g++ main.cpp update_ball.o update_ball_movement.o update_ball_controls.o -o pong.out -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -no-pie
./pong.out
rm *.o *.out &>/dev/null