#!/bin/bash

rm *.txt *.o *.out &>/dev/null
nasm -f elf64 update_ball.s -o update_ball.o
nasm -f elf64 update_ball_movement.s -o update_ball_movement.o
nasm -f elf64 update_key_s.s -o update_key_s.o
nasm -f elf64 update_key_l.s -o update_key_l.o
nasm -f elf64 update_key_c.s -o update_key_c.o
nasm -f elf64 update_key_range.s -o update_key_range.o
nasm -f elf64 limit_movements.s -o limit_movements.o
nasm -f elf64 update_paddle_movement.s -o update_paddle_movement.o
nasm -f elf64 collision_with_ball.s -o collision_with_ball.o
nasm -f elf64 update_ai_paddle.s -o update_ai_paddle.o

g++ demo_asm_game.cpp *.o -Ofast -o asm_pong.out -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -no-pie
./asm_pong.out
rm *.txt *.o *.out &>/dev/null