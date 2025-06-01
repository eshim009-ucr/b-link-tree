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
DEFS=
CFLAGS=-Wall -std=c11 -g $(addprefix -D,$(DEFS))
CXXFLAGS=-Wall -std=c++14 -g $(addprefix -D,$(DEFS))
# Linker Flags
LDLIBS=-lpthread -l:$(LIB)
LDFLAGS=-L.
# Runtime Logs
LOG=main.log thread-logs


### TARGETS & RECIPES ###
.PHONY: all lib docs clean
# Compiler
all: $(EXE)
lib: $(LIB)
# Shell
docs: $(SRC) $(wildcard *.h) $(wildcard tests/*.hpp)
	cd docs && doxygen doxyfile
clean:
	rm -rf $(OBJ) $(EXE) $(LIB) $(LOG)
# Dependencies
main.o: main.cpp $(wildcard tests/*.hpp)
# Manual
$(EXE): $(LIB) $(filter-out $(OBJ_C),$(OBJ))
	$(CXX) -o $@ $^ $(LDFLAGS) $(LDLIBS)
$(LIB): $(OBJ_C)
	$(AR) rcvs $@ $(OBJ_C)
