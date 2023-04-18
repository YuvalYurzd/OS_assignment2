
CC = gcc
CFLAGS = -Wall -g
RM = rm -f
SHAREDLIB = -shared
DLIBFLAGS = -ldl

.PHONY: all clean

all: cmp copy encode decode stshell

cmp: cmp.o
	$(CC) $(CFLAGS) -o $@ $^

copy: copy.o
	$(CC) $(CFLAGS) -o $@ $^


encode: encode.o libcodecA.so libcodecB.so
	$(CC) $(CFLAGS) -o $@ encode.o $(DLIBFLAGS)

decode: decode.o libcodecA.so libcodecB.so
	$(CC) $(CFLAGS) -o $@ decode.o $(DLIBFLAGS)

stshell: stshell.o
	$(CC) $(CFLAGS) -o $@ $^

libcodecA.so: codecA.o
	$(CC) $(CFLAGS) $(SHAREDLIB) -o $@ $^

libcodecB.so: codecB.o
	$(CC) $(CFLAGS) $(SHAREDLIB) -o $@ $^

codecA.o: codecA.c
	$(CC) $(CFLAGS) -fPIC -c $^

codecB.o: codecB.c
	$(CC) $(CFLAGS) -fPIC -c $^

stshell.o: stshell.c
	$(CC) $(CFLAGS) -c $^

%.o: %.c
	$(CC) $(CFLAGS) -c $^

clean:
	$(RM) *.gch *.o *.a *.so *.dll *.dylib cmp copy encode decode stshell