SRC_C=$(wildcard *.c)
SRC_CPP=$(wildcard *.cpp) $(wildcard tests/*.cpp)
SRC=$(SRC_C) $(SRC_CPP)
OBJ_C=$(subst .c,.o,$(SRC_C))
OBJ_CPP=$(subst .cpp,.o,$(SRC_CPP))
OBJ=$(OBJ_C) $(OBJ_CPP)
DEFS=OPTIMISTIC_LOCK
CFLAGS=-Wall -std=c11 -g $(addprefix -D,$(DEFS))
CXXFLAGS=-Wall -std=c++14 -g $(addprefix -D,$(DEFS))
LDLIBS=-lgtest -lpthread


all: test

main.o: main.cpp $(wildcard tests/*.hpp)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

test: $(OBJ)
	$(CXX) -o $@ $^ $(LDLIBS)

.PHONY: docs
docs: $(SRC) $(wildcard *.h) $(wildcard tests/*.hpp)
	cd docs && doxygen doxyfile

clean:
	rm -f $(OBJ) test
