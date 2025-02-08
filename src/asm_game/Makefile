# Compiler and assembler
CXX = g++
NASM = nasm

# Compiler and linker flags
CXXFLAGS = -Wall -Wextra -O2
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -no-pie
NASMFLAGS = -f elf64

# Directories
BUILD_DIR = build
SRC_CPP = asm_game.cpp
SRC_ASM = update_ball.s update_ball_movement.s update_key_s.s update_key_l.s \
          update_ai_paddle.s collision_with_ball.s limit_movements.s \
		  update_paddle_movement.s update_key_c.s update_key_range.s

# Object files in build directory
OBJ_CPP = $(SRC_CPP:%.cpp=$(BUILD_DIR)/%.o)
OBJ_ASM = $(SRC_ASM:%.s=$(BUILD_DIR)/%.o)
OBJ = $(OBJ_CPP) $(OBJ_ASM)

# Output binary
TARGET = pong.out

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Default target
all: $(BUILD_DIR) $(TARGET)

# Compile C++ files into build directory
$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Assemble NASM files into build directory
$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	$(NASM) $(NASMFLAGS) $< -o $@

# Link all object files from build directory into the final executable
$(TARGET): $(OBJ)
	$(CXX) $^ -o $@ $(LDFLAGS)

# Run the game
run: $(TARGET)
	./$(TARGET)

# Clean compiled files
clean:
	rm -rf $(BUILD_DIR) $(TARGET)
