CC := gcc
FLAGS :=  -w
INC_DIR := include
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
INCLUDE := -I./$(INC_DIR)


$(BIN_DIR)/main: $(BUILD_DIR)/main.o $(BUILD_DIR)/test.o $(BUILD_DIR)/RSAES_PKCS1_V1_5_ENCRYPT.o $(BUILD_DIR)/RSAES_PKCS1_V1_5_DECRYPT.o $(BUILD_DIR)/generate_key.o $(BUILD_DIR)/rsa_func.o
	@mkdir -p $(BIN_DIR)
	$(CC) $(FLAGS) $(INCLUDE) $^ -lgmp -lm -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(FLAGS) $(INCLUDE) -lgmp -lm -c -o $@ $<

clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf $(BIN_DIR)
