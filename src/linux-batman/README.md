## How to Build and Install on Linux

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

## 📈 How to Use
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