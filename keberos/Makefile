CC := gcc
FLAGS :=  -w
INC_DIR := include
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
INCLUDE := -I./$(INC_DIR)

all: $(BIN_DIR)/client $(BIN_DIR)/AS $(BIN_DIR)/SS $(BIN_DIR)/TGS

$(BIN_DIR)/client: $(BUILD_DIR)/client.o
	@mkdir -p $(BIN_DIR)
	$(CC) $(FLAGS) $(INCLUDE) $^ -o $@

$(BIN_DIR)/AS: $(BUILD_DIR)/AS.o
	@mkdir -p $(BIN_DIR)
	$(CC) $(FLAGS) $(INCLUDE) $^ -o $@

$(BIN_DIR)/SS: $(BUILD_DIR)/SS.o
	@mkdir -p $(BIN_DIR)
	$(CC) $(FLAGS) $(INCLUDE) $^ -o $@

$(BIN_DIR)/TGS: $(BUILD_DIR)/TGS.o
	@mkdir -p $(BIN_DIR)
	$(CC) $(FLAGS) $(INCLUDE) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(FLAGS) $(INCLUDE) -c -o $@ $<

run:
	./$(BIN_DIR)/AS
	./$(BIN_DIR)/SS
	./$(BIN_DIR)/TGS 
	./$(BIN_DIR)/client

clean:
	@rm -rf $(BIN_DIR)
