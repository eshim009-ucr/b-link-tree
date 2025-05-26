### VARIABLES ###
# Source Code
SRC_C=$(wildcard *.c)
SRC_CPP=$(wildcard *.cpp) $(wildcard tests/*.cpp)
SRC=$(SRC_C) $(SRC_CPP)
# Object Files
OBJ_C=$(subst .c,.o,$(SRC_C))
OBJ_CPP=$(subst .cpp,.o,$(SRC_CPP))
OBJ=$(OBJ_C) $(OBJ_CPP)
# Output Files
EXE=blink
LIB=blink.a
# Compiler Flags
CFLAGS=-Wall -std=c11 -g
CXXFLAGS=-Wall -std=c++14 -g
# Linker Flags
LDLIBS=-lgtest -lpthread -l:$(LIB)
LDFLAGS=-L.


### TARGETS & RECIPES ###
.PHONY: all lib docs clean
# Compiler
all: $(EXE)
lib: $(LIB)
# Shell
docs: $(SRC) $(wildcard *.h) $(wildcard tests/*.hpp)
	cd docs && doxygen doxyfile
clean:
	rm -f $(OBJ) $(EXE) $(LIB)
# Dependencies
main.o: main.cpp $(wildcard tests/*.hpp)
# Manual
$(EXE): $(LIB) $(filter-out $(OBJ_C),$(OBJ))
	$(CXX) -o $@ $^ $(LDFLAGS) $(LDLIBS)
$(LIB): $(OBJ_C)
	$(AR) rcvs $@ $(OBJ_C)
