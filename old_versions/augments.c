#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>			// reading from user database
#include <dirent.h>			// getting directory names and structures

#include "batman.h"

void error(char *report){
	perror(report);
	exit(1);
}

char *getHomeDir(){

	// getting $HOME directory from environment variable HOME
	char *home_dir = getenv("HOME");

	if (home_dir != NULL){
		return home_dir;
	}

	// get $HOME directory from system user database
	uid_t uid = getuid();
	struct passwd *pw = getpwuid(uid);

	if (pw == NULL){
		// failed to get $HOME
		exit(EXIT_FAILURE);
	}
	
	// printf("HOME; %s\n", pw->pw_dir);
	return home_dir;
}

void get_power_supplies(char *power_supplies[BUFFSIZE]){
	/* 
	 * Append an array with directory names of the directories holding the types of power supplies
	 * 	available on the system.
	 *
	 */
	
	struct dirent *dirs;			// pointer for directory entry
	DIR *d = opendir(POWER_SUPPLY_DIR);	// opendir() returns pointer of type DIR

	if ( d ){

		int i = 0;
		while ( (dirs = readdir(d)) != NULL ){

			if ( strcmp(dirs->d_name, ".") == 0 )
				continue;
			else if ( strcmp(dirs->d_name, "..") == 0 )
				continue;
			else{
				power_supplies[i] = dirs->d_name;		// append directory names into array
				printf("power_supplies[%d] = %s\n", i, power_supplies[i]);
				i++;
			}
		}
		printf("value of i: %d\n", i);
		power_supplies[i] = NULL;			// Introduce NULL at end of array
		closedir(d);
	}
}
