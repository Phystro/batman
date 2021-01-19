<h1 align="center"> Batman </h1>
<p align="center">
	Battery AC Technology Monitoring Analysis and Notifications.
</br>
<img src="assets/icons/treetop/Treetog-Junior-Battery-power.ico">
</p>


**Batman** is a convenience utility tool that monitors the battery's energy consumption and usage while displaying notifications alerts to the user in response to changing battery states e.g. charging, discharging, battery absence, low charge capacity e.t.c.

In Linux, on installation, the tool will thereafter always automatically start during the startup of the user's desktop environment after the user has logged in.

The Linux Batman incooporates a small daemon called `batmand` that collects and analyses various power supply parameters. Batmand runs
as root and can also be controlled using systemctl commands.

Currently supported in Linux and Windows based systems.

---
## 🔧 Installation on Windows Based Systems
---
To be able to successfully run Batman on Windows, a few steps are required.
Please follow the guidlines outlined in the [README](src/Batman.Windows/Readme.md) file on building and running batman on Windows.

Consider checking out [LiquidSnow](https://github.com/thismaker/LiquidSnow) framework as it has made this version of `batman` for Windows susccessful. Thanks. 

----
## 🔧 Installation on Linux Based Systems
### How to Build and Install on Linux

Build the project from the sources:
```sh
$ make
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
**Note** More details on installation can be found on the [Build](.github/BUILD.md) file.


**Requirements:** The following packages and libraries are required by `batman` in Linux based systems. 
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

### 📈 How to Use
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

**Note:** If batman is running but unable to display notifications for whatever reason, you can initialize it manually with the command below on your terminal
```sh
batman --events &
```

----

## 💚 Contributing
----
If ANYTHING comes to mind; functionality of the tool, features you would like us to consider, BUGS, etc.. 
**Please just drop it in an issue! Report any bugs encountered**
> Keep in mind that this tool is quite young, and do be a
> helpful so-and-so and report any bugs you encounter.

Read the [Contributing](.github/CONTRIBUTING.md) file on details and guidlines of makeing a contribution to the project. Ensure to adhere to the [Code of Conduct](.github/CODE_OF_CONDUCT.md).
Read the [Security](.github/SECURITY.md) file to see the security policy and how to help in case of a vulnerability.


----
## 📜 License
---

This project is Copyright (c) Anthony Karoki and licensed under the terms of the [GPL-3.0 license](https://choosealicense.com/licenses/gpl-3.0/).
