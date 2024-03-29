CC = gcc
DBG_BIN = gdb
CFLAGS = # -D_GNU_SOURCE
CFLAGS += -std=gnu11
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -pedantic
# CFLAGS += -Werror
CFLAGS += -Wmissing-declarations
ASANFLAGS=-fsanitize=address -fno-common -fno-omit-frame-pointer
CFLAGS += $(shell pkg-config --cflags sdl2 SDL2_image SDL2_ttf)
LDFLAGS = $(shell pkg-config --libs sdl2 SDL2_image SDL2_ttf) -lm
LIBS = -I./libs/

ifeq ($(shell uname -s),Darwin)
	DBG_BIN = lldb
endif

SRC = ./src/*.c
BIN_DIR = ./bin
BIN = $(BIN_DIR)/3drenderer
TEST_DIR = ./tests
TEST_SRC = $(filter-out ./src/main.c, $(wildcard ./src/*.c)) $(TEST_DIR)/*.c

build: bin-dir
	$(CC) $(CFLAGS) $(LIBS) $(SRC) -o $(BIN) $(LDFLAGS)

bin-dir:
	mkdir -p $(BIN_DIR)

ifeq ($(shell uname -s),Darwin)
else
debug: debug-build
	$(DBG_BIN) --args $(BIN) $(ARGS)
endif

debug-build: bin-dir
	$(CC) $(CFLAGS) -g $(LIBS) $(SRC) -o $(BIN) $(LDFLAGS)

run-windowed: build
	@$(BIN) true

run: build
	@$(BIN) $(ARGS)

test:
	$(CC) $(CFLAGS) $(LIBS) $(TEST_SRC) -o $(TEST_DIR)/tests $(LDFLAGS) && $(TEST_DIR)/tests

test-debug:
	$(CC) $(CFLAGS) -g $(LIBS) $(TEST_SRC) -o $(TEST_DIR)/tests $(LDFLAGS) && lldb $(TEST_DIR)/tests $(ARGS)

memcheck:
	@$(CC) $(ASANFLAGS) $(CFLAGS) -g $(LIBS) $(SRC) -o $(BIN_DIR)/memcheck.out $(LDFLAGS)
	@$(BIN_DIR)/memcheck.out
	@echo "Memory check passed"

valgrid:
	$(CC) $(CFLAGS) -g $(LIBS) $(SRC) -o $(BIN_DIR)/memcheck.out $(LDFLAGS)
	valgrind --leak-check=yes $(BIN_DIR)/memcheck.out

leakscheck:
	leaks -atExit -- $(BIN)

clean:
	rm -rf $(BIN_DIR)/* $(TEST_DIR)/tests*

gen-compilation-db:
	bear -- make build

gen-compilation-db-make:
	make --always-make --dry-run \
	| grep -wE 'gcc|g\+\+' \
	| grep -w '\-c' \
	| jq -nR '[inputs|{directory:".", command:., file: match(" [^ ]+$").string[1:]}]' \
	> compile_commands.json
