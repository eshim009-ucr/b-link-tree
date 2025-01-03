SRC=$(wildcard *.c)
OBJ=$(subst .c,.o,$(SRC)) main.o
CFLAGS=-Wall
LDLIBS=-lgtest


all: test

tree.o: tree.c tree.h types.h defs.h
io.o: io.c tree.h types.h defs.h
%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

main.o: main.cpp test.hpp
	$(CXX) -c $(CFLAGS) -o $@ $<

test: $(OBJ)
	$(CXX) -o $@ $^ $(LDLIBS)

.PHONY: docs
docs: $(SRC) $(wildcard *.cpp) $(wildcard *.h) $(wildcard *.hpp)
	cd docs && doxygen doxyfile

clean:
	rm -f $(OBJ) test
