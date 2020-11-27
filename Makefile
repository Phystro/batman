CC=gcc
CFLAGS=-Wall -g

all: batman

install:
	cp batman /usr/local/bin/

uninstall:
	rm -rf /usr/local/bin/batman
	rm -rf *.o batman

batman: batman.o augments.o filehandler.o uevent.o
	$(CC) $(CFLAGS) batman.o augments.o filehandler.o uevent.o -o batman

# Automatic variables
%.o: %.c
	$(CC) $(CLFAGS) -c $^

clean:
	rm *.o batman

