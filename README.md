# Batman
Energy consumption, power usage and battery monitoring program for Linux systems.



#### Functions
What it does

 - Notifies of battery events e.g. notifications for low charge, full charge, discharging...
 - Monitors battery health and capacity
 - Monitors power usage


#### Installation

Ensure the following dependancies and libraries are installed
 * libnotify-dev
 * libao-dev
 * libmpg123-dev

```sh
$ apt install libnotify
$ apt install libao-dev
$ apt install libmpg123
```

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


#### Work in Progress
 - fixing notifications
 - adding alert sounds


> Keep in mind that this tool is quite young, and do be a
> helpful so-and-so and report any bugs you encounter on
> the GitHub project page.

**Still a work in progress!**


