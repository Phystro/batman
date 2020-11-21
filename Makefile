CC=gcc
CFLAGS=-Wall -g

all: batman

install: cp batman /usr/local/bin/

batman: batman.o augments.o 
	$(CC) $(CFLAGS) batman.o augments.o -o batman

# Automatic variables
%.o: %.c
	$(CC) $(CLFAGS) -c $^

clean:
	rm *.o batman
