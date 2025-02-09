#!/bin/bash

rm *.txt *.o *.out &>/dev/null

g++ cpp_game.cpp -O0 -o cpp_pong.out -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./cpp_pong.out

rm *.txt *.o *.out &>/dev/null