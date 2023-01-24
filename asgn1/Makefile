CC = clang
CFLAGS = -Wall -Werror -Wpedantic -Wextra
LFLAGS = -lm

all: httpserver

httpserver: httpserver.o bind.o
	$(CC) -o httpserver httpserver.o bind.o $(LFLAGS)

httpserver.o: httpserver.c
	$(CC) $(CFLAGS) -c httpserver.c
bind.o: bind.c
	$(CC) $(CFLAGS) -c bind.c

format:
	clang-format -i -style=file *.c

clean:
	rm -f httpserver *.o

