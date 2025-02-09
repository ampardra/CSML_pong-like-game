#!/bin/bash

# Run script1.sh in the background
cd ./cpp_game
./demo_game.sh &
cd ..

# Run script2.sh in the background
cd ./asm_game
./demo_game.sh &
cd ..

# Wait for both scripts to finish
wait

echo "Both scripts have finished execution."