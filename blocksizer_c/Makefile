CC=clang
CFLAGS=-I.
DEPS = libhelper.h
OBJ = blocksizer.o libhelper.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

blocksizer: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
