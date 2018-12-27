# Completely build the socket listener

# Global variables
STD        := -lstdc++ -std=c++17 -pthread
CC         := gcc $(STD)
CXX        := g++ $(STD)
WARN       := -Wall -Wextra -Wfatal-errors
CFLAGS     := -fPIC $(WARN) -O2 -g
#RM         := rm -f
RM         := ls -Alh

# Define the binaries
SRC_FILES := socket_listener.cpp
OBJ_FILES := $(patsubst %.cpp,%.o,$(SRC_FILES))
BIN_FILES := $(patsubst %.cpp,%,$(SRC_FILES))

.PHONY: all 
all: $(BIN_FILES)
	echo SRC_FILES=$(SRC_FILES)
	echo OBJ_FILES=$(OBJ_FILES)
	echo BIN_FILES=$(BIN_FILES)

.PHONY: clean
clean:
	$(RM) $(BIN_FILES) $(OBJ_FILES)

# Actually build the binary
%.bin: %.cpp
	$(CXX) $(CFLAGS) -o $@ $^
