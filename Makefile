CC=gcc
CFLAGS=-Wall -g -std=c99
LIBS=`pkg-config --cflags --libs libnotify`
AO_MP3=-lmpg123 -lao
HOMEDIR=${HOME}

all: batman batmand

install:
	sudo cp batman /usr/local/bin/
	sudo cp batmand /usr/local/bin/
	sudo mkdir /usr/share/pixmaps/batman
	sudo mkdir /usr/share/sounds/batman
	sudo cp -rf ./icons/ /usr/share/pixmaps/batman/
	sudo cp -rf ./sounds/ /usr/share/sounds/batman/

	echo
	./build_service.sh

	echo
	echo "[+] Installing the batman and batmand service unit configuration files"
	# sudo cp batman.service /etc/systemd/system/
	sudo cp batmand.service /etc/systemd/system/
	sudo cp batman-autostart.desktop /etc/xdg/autostart/
	sudo cp batman-autostart.desktop ~/.config/autostart/

	echo
	echo "[+] Reloading unit configuration file definitions"
	sudo systemctl daemon-reload
	
	echo
	echo "[+] Enabling batman and batmand daemon launch at startup"
	# sudo systemctl enable batman.service
	echo
	sudo systemctl enable batmand.service
	
	echo
	echo "[+] Launching batman and batmand daemon service"
	sudo systemctl start batmand.service
	echo
	# sudo systemctl start batman.service
	
	echo
	echo "[+] Batman and Batmand daemon service status"
	# sudo systemctl status batman.service | cat
	echo
	sudo systemctl status batmand.service | cat

uninstall:
	sudo rm -rf /usr/local/bin/batman
	sudo rm -rf /usr/local/bin/batmand

	sudo rm -rf *.o *.service batman batmand

	# sudo rm -rf ./batman.service
	sudo rm -rf ./batmand.service

	# sudo systemctl disable batman.service
	sudo systemctl disable batmand.service

	# sudo systemctl stop batman.service
	sudo systemctl stop batmand.service

	# sudo rm -rf /etc/systemd/system/batman.service
	sudo rm -rf /etc/systemd/system/batmand.service

purge:
	sudo rm -rf /usr/local/bin/batman
	sudo rm -rf /usr/local/bin/batmand

	sudo rm -rf *.o *.service batman batmand

	# sudo rm -rf ./batman.service
	sudo rm -rf ./batmand.service

	sudo rm -rf /var/lib/batman

	sudo rm -rf /usr/share/pixmaps/batman
	sudo rm -rf /usr/share/sounds/batman

	# sudo systemctl disable batman.service
	sudo systemctl disable batmand.service

	# sudo systemctl stop batman.service
	sudo systemctl stop batmand.service

	# sudo rm -rf /etc/systemd/system/batman.service
	sudo rm -rf /etc/systemd/system/batmand.service


batman: batman.o augments.o filehandler.o uevent.o
	sudo $(CC) $(CFLAGS)  batman.o augments.o filehandler.o uevent.o -o batman $(LIBS) $(AO_MP3)

batmand: devent.o augments.o filehandler.o
	sudo $(CC) $(CFLAGS) devent.o augments.o filehandler.o -o batmand $(LIBS) $(AO_MP3)

# Automatic variables
%.o: src/%.c
	sudo $(CC) $(CLFAGS)  -c $^ $(LIBS) $(AO_MP3)

clean:
	sudo rm *.o batman batmand

