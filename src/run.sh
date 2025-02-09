#!/bin/bash

# Number of times to run the game
MAX_RUNS=20  # Change this to the number of times you want to run the game

# Loop to run the game multiple times
for ((i=1; i<=MAX_RUNS; i++)); do
    echo "Running game iteration $i..."
    ./run_asm.sh  # Run the game and close it after 10 seconds
    echo "Game iteration asm $i completed."
    sleep 2
done
for ((i=1; i<=MAX_RUNS; i++)); do
    echo "Running game iteration $i..."
    ./run_cpp.sh  # Run the game and close it after 10 seconds
    echo "Game iteration cpp $i completed."
    sleep 2  # Optional: Add a delay between iterations
    sleep 2
done

echo "All game iterations completed."