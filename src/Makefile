CC 		= gcc
CFLAGS 	= -I /usr/include/python2.7/ -lm -lpthread -lpython2.7 -I ../lib/  

OBJECTS = oneline.o websocket.o  json.o
EXEC 	= onelinev2-server

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

json.o: json.c json.h
	$(CC) $(CFLAGS) -c json.c

main: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(EXEC) main.c
	cp -rf $(EXEC) /usr/bin/$(EXEC)
       

