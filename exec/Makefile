### VARIABLES ###
# Source Code
SRC=$(wildcard *.cpp)
CORE_PATH=../core
# Object Files
OBJ=$(subst .cpp,.o,$(SRC))
# Output Files
EXE=blink
# Compiler Flags
DEFS=
CXXFLAGS=-Wall -std=c++14 -g $(addprefix -D,$(DEFS)) -I$(CORE_PATH)
# Linker Flags
LIB=blink.so
LDLIBS=-l:$(LIB) -lpthread
LDFLAGS=-L$(CORE_PATH)


### TARGETS & RECIPES ###
.PHONY: all lib clean test
# Compiler
all: $(EXE)
	@echo ' ___________________________________________'
	@echo '/                                            \'
	@echo '|  Run with LD_LIBRARY_PATH=$(CORE_PATH) ./$(EXE)  |'
	@echo '\____________________________________________/'
# Shell
clean:
	rm -rf $(OBJ) $(EXE) $(LIB) $(LOG)
# Dependencies
main.o: main.cpp
$(OBJ): $(CORE_PATH)/defs.h $(CORE_PATH)/types.h

# Manual
$(EXE): $(OBJ) $(LIB)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(LDFLAGS) $(LDLIBS)
$(LIB):
	$(MAKE) -C $(CORE_PATH)
