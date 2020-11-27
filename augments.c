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


char *get_home_dir(){

	// getting $HOME directory from environment variable HOME
	char *home_dir = getenv("HOME");

	if (home_dir != NULL){
		printf("HOME dir from environment: %s\n", home_dir);
		return home_dir;
	}

	// get $HOME directory from system user database
	uid_t uid = getuid();
	struct passwd *pw = getpwuid(uid);

	if (pw == NULL){
		// failed to get $HOME
		exit(EXIT_FAILURE);
	}

	printf("HOME from user db: %s\n", pw->pw_dir);
	return pw->pw_dir;
}


void get_power_modes( char *power_modes[] ){

	/*
	 * Append an array with the directory names of directories holding the types/modes of power supplies
	 * 	available on the system.
	 */

	struct dirent *dirs;					// pointer for directory entry
	DIR *d = opendir( POWER_SUPPLY_DIR );			// opendir() returns a pointer of type DIR

	if ( d ){
	
		int i = 0;
		while ( ( dirs = readdir( d ) ) != NULL ){
		
			if ( strncmp( dirs->d_name, ".", 1 ) == 0 )
				continue;
			else if ( strncmp( dirs->d_name, "..", 2 ) == 0 )
				continue;
			else {
				power_modes[i] = dirs->d_name;		// append directory names into array
				i++;
			}
		}
		power_modes[i] = NULL;					// introduce NULL at end of array
		closedir( d );
	}
}


void display_usage(){

	printf( "\nBatman v0.0.1\nEnergy consumption, power usage and battery monitoring tool.\n\nUSAGE:\n\tbatman [OPTIONS]...\n\nOPTIONS:\n\t-i, --info\t\tDisplay summarised battery and/or AC Mains information \n\t-s, --stats\t\tDisaply battery or AC Mains stats information \n\t    --start_daemon\tStart batman daemon if the daemon was stopped \n\t    --stop_daemon\tStop batman daemon if the daemon is running  \n" );
	
	exit( EXIT_FAILURE );
}

