HOMEDIR=$HOME
USERNAME=$(id -un)


sudo touch batman.service
sudo touch batmand.service
sudo chmod +666 batman.service
sudo chmod +666 batmand.service


############## BATMAN.SERVICE #########################################

echo "[Unit]" > batman.service
echo "Description=Battery AC Technology Monitoring Analysis and Notifications." >> batman.service
echo "Wants=syslog.service" >> batman.service
echo "After=systemd-user-sessions.service systemd-journald.service" >> batman.service
echo "Requires=sound.target" >> batman.service
echo >> batman.service

echo >> batman.service
echo "[Service]" >> batman.service
echo "Type=simple" >> batman.service
echo "User=$USERNAME" >> batman.service
echo "SyslogIdentifier=$USERNAME" >> batman.service
echo "Environment=\"DISPLAY=:0\"" >> batman.service
echo "Environment=\"XAUTHORITY=$HOMEDIR/.Xauthority\"" >> batman.service
echo "ExecStart=/usr/local/bin/batman --events" >> batman.service
echo "RestartSec=10" >> batman.service
echo "Restart=on-failure" >> batman.service
echo "IgnoreSIGPIPE=no" >> batman.service
echo >> batman.service
# echo "StandardOutput=syslog" >> batman.service
# echo "StandardError=syslog" >> batman.service
echo "KillMode=process" >> batman.service
echo >> batman.service

echo >> batman.service
echo "[Install]" >> batman.service
echo "WantedBy=default.target graphical.target sound.target" >> batman.service
echo >> batman.service



############# BATMAND.SERVICE #########################################

echo "[Unit]" > batmand.service
echo "Description=Battery AC Technology Monitoring Analysis and Notifications Daemon." >> batmand.service
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
