CC=g++
CFLAGS=-Wall -Wextra -Isrc/include --std=c++17
DEBUG_FLAGS=-O0 -ggdb -fsanitize=address
RELEASE_FLAGS=-O3
SRC= src/wfc_tile.cpp \
		 src/wfc_canvas.cpp
MAIN=src/main.cpp
OUT=-o out/wfc
TEST_OUT=-o out/wfc-test
LIB=
ARGS=
NAME=

pre:
	mkdir -p out

debug: pre
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(OUT) $(SRC) $(MAIN) $(LIB)

release: pre
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) $(OUT) $(SRC) $(MAIN) $(LIB)

test: pre
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(TEST_OUT) $(SRC) ./tests/test_wfc_$(NAME).cpp $(LIB)

runDebug: debug
	cd out && ./wfc $(ARGS)

runRelease: release
	cd out && ./wfc $(ARGS)

runTest: test
	cd out && ./wfc-test $(ARGS)
