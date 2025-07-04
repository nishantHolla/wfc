CC=g++
CFLAGS=-Wall -Wextra -Isrc/include --std=c++17
DEBUG_FLAGS=-O0 -ggdb -fsanitize=address
RELEASE_FLAGS=-O3
SRC=src/main.cpp
OUT=-o out/wfc
LIB=
ARGS=

pre:
	mkdir -p out

debug: pre
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(OUT) $(SRC) $(LIB)

release: pre
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) $(OUT) $(SRC) $(LIB)

runDebug: debug
	cd out && ./wfc $(ARGS)

runRelease: release
	cd out && ./wfc $(ARGS)
