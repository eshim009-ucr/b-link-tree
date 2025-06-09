### VARIABLES ###
# Source Code
SRC=$(wildcard *.c)
# Object Files
OBJ=$(subst .c,.o,$(SRC))
# Output Files
LIB=blink.so
# Compiler Flags
DEFS=
CFLAGS=-Wall -std=c11 -g $(addprefix -D,$(DEFS))
CXXFLAGS=-Wall -std=c++14 -g $(addprefix -D,$(DEFS))


### TARGETS & RECIPES ###
.PHONY: all clean
# Compiler
all: $(LIB)
# Shell
clean:
	rm -rf $(OBJ) $(LIB)
# Dependencies
$(OBJ): defs.h types.h
# Manual
%.o: %.c
	$(CC) -fpic -shared -c $(CFLAGS) -o $@ $<
$(LIB): $(OBJ)
	$(CC) -fpic -shared -o $@ $^ $(CFLAGS) $(LDFLAGS) $(LDLIBS)
