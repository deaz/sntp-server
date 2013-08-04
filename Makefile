CC=gcc
CFLAGS=-c -Wall -g
LDFLAGS=
SOURCES=main.c daemon.c server.c
OBJECTS=$(SOURCES:.c=.o) 
EXECUTABLE=sntpd

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf *.o $(EXECUTABLE)
