CC=gcc
CFLAGS=-Wall -g
LIB=`pkg-config --cflags --libs libnotify`

all: batman

install:
	cp batman /usr/local/bin/

uninstall:
	rm -rf /usr/local/bin/batman
	rm -rf *.o batman

batman: batman.o augments.o filehandler.o uevent.o devent.o
	$(CC) $(CFLAGS)  batman.o augments.o filehandler.o uevent.o devent.o -o batman $(LIB)

# Automatic variables
%.o: %.c
	$(CC) $(CLFAGS)  -c $^ $(LIB)

clean:
	rm *.o batman

