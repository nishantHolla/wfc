CC=g++
CFLAGS=-Wall -Wextra -Isrc/include --std=c++17
DEVELOP_FLAGS=-O0 -ggdb -fsanitize=address
DEBUG_FLAGS=-O0 -ggdb
TEST_FLAGS=-O0 -ggdb
RELEASE_FLAGS=-O3
SRC= src/wfc_tile.cpp \
		 src/wfc_canvas.cpp \
		 src/wfc_sdl_utils.cpp \
		 src/wfc_parser.cpp \
		 src/wfc_directions.cpp \
		 src/wfc_random.cpp \
		 src/wfc.cpp
MAIN=src/main.cpp
OUT=-o out/wfc
TEST_OUT=-o out/wfc-test
LIB=$(shell pkg-config --libs sdl2) -lSDL2_image
ARGS=
NAME=

pre:
	rm -r ./out
	mkdir -p ./out
	cd out && ln -s ../examples . && cd ..

develop: pre
	$(CC) $(CFLAGS) $(DEVELOP_FLAGS) $(OUT) $(SRC) $(MAIN) $(LIB)

debug: pre
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(OUT) $(SRC) $(MAIN) $(LIB)

release: pre
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) $(OUT) $(SRC) $(MAIN) $(LIB)

test: pre
	$(CC) $(CFLAGS) $(TEST_FLAGS) $(TEST_OUT) $(SRC) ./tests/test_wfc_$(NAME).cpp $(LIB)

runDevelop: develop
	cd out && ./wfc $(ARGS)

runDebug: debug
	cd out && ./wfc $(ARGS)

runRelease: release
	cd out && ./wfc $(ARGS)

runTest: test
	cd out && ./wfc-test $(ARGS)
