LIB=blink.a
EXE=gen-bench

all: $(EXE)

$(EXE): $(wildcard *.c) ../$(LIB)
	$(CC) -o $@ $^ -g -L.. -l:$(LIB) -I..

../$(LIB):
	make -C .. $(notdir $@)

clean:
	rm -rf *.bin $(EXE)
