### VARIABLES ###
# Source Code
SRC_HPP=$(wildcard *.hpp)
SRC_CPP=$(wildcard *.cpp)
SRC=$(SRC_HPP) $(SRC_CPP)
CORE_PATH=../core
# Object Files
OBJ=$(subst .cpp,.o,$(SRC_CPP))
# Output Files
EXE=blink-test
# Compiler Flags
DEFS=
CXXFLAGS=-Wall -std=c++14 -g $(addprefix -D,$(DEFS)) -I$(CORE_PATH)
# Linker Flags
LIB=blink.so
LDLIBS=-l:$(LIB) -lgtest
LDFLAGS=-L$(CORE_PATH)
# Runtime Logs
LOG=main.log thread-logs


### TARGETS & RECIPES ###
.PHONY: all lib clean test
# Compiler
all: $(EXE)
# Shell
clean:
	rm -rf $(OBJ) $(EXE) $(LIB) $(LOG)
test: $(EXE)
	LD_LIBRARY_PATH=$(CORE_PATH) ./$(EXE)
# Dependencies
main.o: main.cpp $(SRC_HPP)
$(OBJ): $(CORE_PATH)/defs.h $(CORE_PATH)/types.h

# Manual
$(EXE): $(OBJ) $(LIB)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(LDFLAGS) $(LDLIBS)
$(LIB):
	$(MAKE) -C $(CORE_PATH)
