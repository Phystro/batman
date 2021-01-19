# Build and Run on Linux

To understand how batman and batmand works on Linux or to debug an issue, get all the source files, then locally build and run it.

## Obtain the Source Code

First, fork the batman repository so that you can make a pull request. Clone the forked repository to your local machine.

```
git clone https://github.com/{username}/batman.git
```

**Note** Ensure that you've installed all dependancies and libraries. You can reference them from the [requirements.txt](../requirements.txt) file


### Building the Project
```
make
```

### Installing from The Source Code
```
make install
```

### Uninstalling
```
make uninstall
```

### Cleaning
```
make clean
```

### Uninstall and Delete The Whole Build
```
make purge
```


# Running
Batmand runs automatically through systemd, thus use the systemd commands to control it;
```
systemctl start batmand.sevice
systemctl stop batmand.service
systemctl restart batmand.service
systemctl status batmand.service
systemctl disable batmand.service
systemctl enable batmand.service
```

Batman used to run automatically through systemd but we got a few issues, especially on sound. We thus opted to run it at startup after login using the user's desktop environment such as gnome.


If batman fails to launch at startup, you can either use systemd commands to restart it or run it manually as a background process using the commands;
```
batman --events &
```


