#!/bin/bash

# Start your game in the background
cd ./cpp_game
./demo_game.sh &
GAME_PID=$!  # Store the process ID of the game

# Wait for 10 seconds
sleep 5

# Simulate the Esc key press to close the game
xdotool key Escape

# Wait for the game process to exit
wait $GAME_PID

echo "Game closed automatically after 10 seconds."

cd ..