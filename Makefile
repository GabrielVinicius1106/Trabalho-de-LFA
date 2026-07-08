CXX = g++

SRC_DIR = src
OUT_DIR = out

SRC = main.cpp $(SRC_DIR)/leitor.cpp $(SRC_DIR)/gramatica.cpp $(SRC_DIR)/cyk.cpp
BIN = $(OUT_DIR)/a

all: $(BIN)

$(BIN): $(SRC) | $(OUT_DIR)
	@$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN)

$(OUT_DIR):
	@mkdir -p $(OUT_DIR)

run: $(BIN)
	@./$(BIN) data/exemplo.glc

clean:
	@rm -rf $(OUT_DIR)

.PHONY: all run clean