HOMEDIR=$HOME
USERNAME=$(id -un)


sudo touch batmand.service
sudo chmod +666 batmand.service


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


sudo chmod -022 batmand.service
