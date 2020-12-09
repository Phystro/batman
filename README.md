# Batman
Batman is background tool that monitors the battery's energy consumption and usage. Batman will display notifications to the
user in response to changing battery states e.g. charging, discharging, battery absence, low charge capacity e.t.c.

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
$ sudo make
```
or
```sh
$ sudo make all
```

Install the project:

```sh
$ sudo make install
```

##### Uninstalling
Uninstalling
```sh
$ sudo make uninstall
```
Uninstall and delete all associated `batman` program files
```sh
$ sudo make purge
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

Starting the batman daemon:
```sh
batman --start_daemon
batman --start
```

Stopping the batman daemon
```sh
batman --stop_daemon
batman --stop
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

