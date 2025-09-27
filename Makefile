### VARIABLES ###
# Source Code
SRC_LIB=insert.cpp io.cpp memory.cpp node.cpp operations.cpp search.cpp tree-helpers.cpp util.cpp validate.cpp
SRC_EXE=$(wildcard *.cpp) $(wildcard tests/*.cpp)
SRC=$(SRC_LIB) $(SRC_EXE)
# Object Files
OBJ_LIB=$(subst .cpp,.o,$(SRC_LIB))
OBJ_EXE=$(subst .cpp,.o,$(SRC_EXE))
OBJ=$(OBJ_LIB) $(OBJ_EXE)
# Output Files
EXE=blink
LIB=blink.a
# Compiler Flags
DEFS=
CFLAGS=-Wall -std=c11 -g $(addprefix -D,$(DEFS))
CXXFLAGS=-Wall -std=c++14 -g $(addprefix -D,$(DEFS))
# Linker Flags
LDLIBS=-lpthread -l:$(LIB) $(if $(wildcard /usr/lib/libgtest.so),-lgtest)
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
$(OBJ): defs.h types.h
main.o: main.cpp $(wildcard tests/*.hpp)
# Manual
$(EXE): $(LIB) $(filter-out $(OBJ_LIB),$(OBJ))
	$(CXX) -o $@ $^ $(LDFLAGS) $(LDLIBS)
$(LIB): $(OBJ_LIB)
	$(AR) rcvs $@ $(OBJ_LIB)
