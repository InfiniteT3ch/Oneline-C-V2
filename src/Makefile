CC 		= gcc
CFLAGS 	= -I /usr/local/include/python2.7/ -lpthread -I ../lib/ 

OBJECTS = /usr/lib/libpython2.7.so.1.0  oneline.o websocket.o
EXEC 	= main

.PHONY:  main

all: clean  main

clean:
	rm -f $(EXEC) *.o

run: all
	./$(EXEC) $(PORT)

oneline.o: oneline.c oneline.h
	$(CC) $(CFLAGS) -c oneline.c
websocket.o: websocket.c websocket.h
	$(CC) $(CFLAGS) -c websocket.c

main: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o main main.c

