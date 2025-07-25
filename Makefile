CC = g++
CFLAGS = -IFrontend/include -IBackend/include -Wall -Wextra

TARGET = run

SRC_DIR_FRONTEND = ./Frontend/src
SRC_DIR_BACKEND = ./Backend/src

BUILD_DIR_MAIN = ./build
BUILD_DIR_FRONTEND = ./Frontend/build
BUILD_DIR_BACKEND = ./Backend/build

BIN_DIR = ./bin
DUMP_DIR = ./Frontend/dump

SRC_MAIN = ./main.cpp
SRC_FRONTEND = $(SRC_DIR_FRONTEND)/vector.cpp $(SRC_DIR_FRONTEND)/tokenizer.cpp $(SRC_DIR_FRONTEND)/parser.cpp $(SRC_DIR_FRONTEND)/tree.cpp 
SRC_BACKEND = $(SRC_DIR_BACKEND)/asmCommands.cpp $(SRC_DIR_BACKEND)/generator.cpp $(SRC_DIR_BACKEND)/headers.cpp $(SRC_DIR_BACKEND)/standartFunctions.cpp

OBJ_MAIN = $(BUILD_DIR_MAIN)/main.o
OBJ_FRONTEND = $(BUILD_DIR_FRONTEND)/vector.o $(BUILD_DIR_FRONTEND)/tokenizer.o $(BUILD_DIR_FRONTEND)/parser.o $(BUILD_DIR_FRONTEND)/tree.o
OBJ_BACKEND = $(BUILD_DIR_BACKEND)/asmCommands.o $(BUILD_DIR_BACKEND)/generator.o $(BUILD_DIR_BACKEND)/headers.o $(BUILD_DIR_BACKEND)/standartFunctions.o

$(BIN_DIR)/$(TARGET): $(OBJ_MAIN) $(OBJ_FRONTEND) $(OBJ_BACKEND)
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CFLAGS) $^ -o $@
	
$(BUILD_DIR_MAIN)/main.o: $(SRC_MAIN)
	@mkdir -p $(BUILD_DIR_MAIN)
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR_FRONTEND)/vector.o: $(SRC_DIR_FRONTEND)/vector.cpp
	@mkdir -p $(BUILD_DIR_FRONTEND)
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR_FRONTEND)/tokenizer.o: $(SRC_DIR_FRONTEND)/tokenizer.cpp
	@mkdir -p $(BUILD_DIR_FRONTEND)
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR_FRONTEND)/parser.o: $(SRC_DIR_FRONTEND)/parser.cpp
	@mkdir -p $(BUILD_DIR_FRONTEND)
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR_FRONTEND)/tree.o: $(SRC_DIR_FRONTEND)/tree.cpp
	@mkdir -p $(BUILD_DIR_FRONTEND)
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR_BACKEND)/asmCommands.o: $(SRC_DIR_BACKEND)/asmCommands.cpp
	@mkdir -p $(BUILD_DIR_BACKEND)
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR_BACKEND)/generator.o: $(SRC_DIR_BACKEND)/generator.cpp
	@mkdir -p $(BUILD_DIR_BACKEND)
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR_BACKEND)/headers.o: $(SRC_DIR_BACKEND)/headers.cpp
	@mkdir -p $(BUILD_DIR_BACKEND)
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR_BACKEND)/standartFunctions.o: $(SRC_DIR_BACKEND)/standartFunctions.cpp
	@mkdir -p $(BUILD_DIR_BACKEND)
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean run

clean:
	@rm -rf $(BIN_DIR) $(BUILD_DIR_MAIN) $(BUILD_DIR_FRONTEND) $(BUILD_DIR_BACKEND) $(DUMP_DIR)

run: $(BIN_DIR)/$(TARGET)
	@mkdir -p $(DUMP_DIR)
	@$(BIN_DIR)/$(TARGET)