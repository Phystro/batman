CC=gcc
CFLAGS=-Wall -g
LIBS=`pkg-config --cflags --libs libnotify`

all: batman

install:
	cp batman /usr/local/bin/
	mkdir /usr/share/pixmaps/batman
	cp -rf ./icons/ /usr/share/pixmaps/batman/

uninstall:
	rm -rf /usr/local/bin/batman
	rm -rf *.o batman

purge:
	rm -rf /usr/local/bin/batman
	rm -rf *.o batman
	rm -rf ~/.batman
	rm -rf /usr/share/pixmaps/batman


batman: batman.o augments.o filehandler.o uevent.o devent.o
	$(CC) $(CFLAGS)  batman.o augments.o filehandler.o uevent.o devent.o -o batman $(LIBS)

# Automatic variables
%.o: %.c
	$(CC) $(CLFAGS)  -c $^ $(LIBS)

clean:
	rm *.o batman

