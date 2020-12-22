# Batman
Batman is convenience tool that monitors the battery's energy consumption and usage. Batman will display notifications to the
user in response to changing battery states e.g. charging, discharging, battery absence, low charge capacity e.t.c.

On installation, the tool will ran at startup using systemd to spawn notifications upon battery/power supply events.
One can thususe systemctl commands to control it.
Batman incooporates a small daemon called `batmand` that collects and analyses various power supply parameters. Batmand runs
as root and can also be controlled using systemctl commands.

Currently supported in Linux systems.


#### Functions
What it does

 - Notifies of battery events e.g. notifications for low charge, full charge, discharging...
 - Monitors battery health and capacity
 - Monitors power usage


#### Installation
##### How to install

Build the project:
```sh
$ make
```
or
```sh
$ make all
```

Install the project:

```sh
$ make install
```

##### Uninstalling
Uninstalling
```sh
$ make uninstall
```
Uninstall and delete all associated `batman` program files
```sh
$ make purge
```

**Requirements:** The following packages and libraries are required by `batman`. 
```sh
   `libnotify-dev`, `libao-dev`, `libmpg123-dev`
```
```sh
$ sudo apt install libnotify-dev libao-dev libmpg123-dev
```
**Note:** Remember to update the necessary packages and dependancies
```sh
$ sudo apt install gcc build-essential
```

----

#### How to Use
Getting Power Supply Info:
```sh
batman -i
batman --info
```

Getting Power Supply Stats:
```sh
batman -s
batman --stats
```

----


#### Work in Progress
 - fixing notifications
 - adding alert sounds

----
**Still a work in progress!**

----
### License

This project is Copyright (c) Anthony Karoki and licensed under the terms of the GPL-3.0 license.

----
## Contributing
If ANYTHING comes to mind; functionality of the tool, features you would like us to consider, BUGS, etc.. 
**Please just drop it in an issue! Report any bugs encountered**
> Keep in mind that this tool is quite young, and do be a
> helpful so-and-so and report any bugs you encounter.

