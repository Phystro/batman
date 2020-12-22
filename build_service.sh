HOMEDIR=$HOME
USERNAME=$(id -un)


sudo touch batman.service
sudo touch batmand.service
sudo chmod +666 batman.service
sudo chmod +666 batmand.service


############## BATMAN.SERVICE #########################################

echo "[Unit]" > batman.service
echo "Description=Power usage and battery events notifications and statistics." >> batman.service
echo "Wants=syslog.service" >> batman.service
echo >> batman.service

echo >> batman.service
echo "[Service]" >> batman.service
echo "User=$USERNAME" >> batman.service
echo "Environment=\"DISPLAY=:0\"" >> batman.service
echo "Environment=\"XAUTHORITY=$HOMEDIR/.Xauthority\"" >> batman.service
echo "ExecStart=batman --events" >> batman.service
echo "RestartSec=10" >> batman.service
echo "Restart=on-failure" >> batman.service
echo >> batman.service
echo "KillMode=process" >> batman.service
echo >> batman.service

echo >> batman.service
echo "[Install]" >> batman.service
echo "WantedBy=multi-user.target graphical.target" >> batman.service
echo >> batman.service



############# BATMAND.SERVICE #########################################

echo "[Unit]" > batmand.service
echo "Description=Energy consumption, power usage and battery monitoring tool daemon." >> batmand.service
echo "Wants=syslog.service" >> batmand.service
echo "After=syslog.service" >> batmand.service
echo "Requires=dbus.service" >> batmand.service
echo >> batmand.service

echo >> batmand.service
echo "[Service]" >> batmand.service
echo "Type=forking" >> batmand.service
echo "PIDFile=/var/run/batmand.pid" >> batmand.service
echo "ExecStart=batmand" >> batmand.service
echo "RestartSec=10" >> batmand.service
echo "Restart=on-failure" >> batmand.service
echo >> batmand.service
echo "KillMode=process" >> batmand.service
echo >> batmand.service

echo >> batmand.service
echo "[Install]" >> batmand.service
echo "WantedBy=multi-user.target" >> batmand.service
echo >> batmand.service



sudo chmod -022 batman.service
sudo chmod -022 batmand.service
