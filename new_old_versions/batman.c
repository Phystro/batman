/*
 * Name:	Batman
 * Version:	0.0.1
 * Author:	Anthony Karoki
 * Alias:	PhyTensor
 * Description:	Energy consumption, power usage and battery monitoring program for Linux systems.
 * Copyright (C) (2020) Anthony Karoki - Eccentric Tensor Labs
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <sys/stat.h>			// checking if directory exists
#include <sys/types.h>

#include "batman.h"


static void batman_daemon(){

	/* Open the log file for logging */
	openlog("batmand", LOG_PID, LOG_DAEMON);
}

int main(int argc, char *argv[]){

	/* Parsing user input arguments */



	/* If first time run, create a home working directory */
	char *home_dir = get_home_dir();
	char batman_work_dir[BUFFSIZE];

	strcpy(batman_work_dir, home_dir);
	strcat(batman_work_dir, "/.batman/");
	printf("home: %s\nwork_dir: %s\n", home_dir, batman_work_dir);

	// create working dir if not present
	struct stat st = {0};
	if ( stat(batman_work_dir, &st) == -1 ){
		mkdir(batman_work_dir, 0644);				// rw_r__r__ permissions
	}


	
	/* If first time run, create power modes directories on home working directory */
	char temp_dir[BUFFSIZE];
	char *power_modes[BUFFSIZE];
	*power_modes = get_power_modes(power_modes);

	for ( int i = 0; ; i++) {
		
		if ( *(power_modes + i) == NULL ){
			printf("end of power modes\n");
			break;
		}

		printf("power_modes[%d]: %s\n", i, *(power_modes + i) );
		printf("temp_dir start: %s\n", temp_dir);

		strcpy(temp_dir, batman_work_dir);
		strcat(temp_dir, *(power_modes + i));
		printf("temp-dir: %s\n", temp_dir);

		if ( stat(temp_dir, &st) == -1 ){
			mkdir(temp_dir, 0644);
			// create_data_files(temp_dir);
		}

		printf("value of temp_dir-before: %s\n", temp_dir);
		memset(temp_dir, 0, sizeof temp_dir);
		printf("value of temp_dir-after: %s\n", temp_dir);
	}


	/* Start BATMAN Daemon */
	batman_daemon();
	

	/* Spawn Daemon Activities */
	syslog(LOG_NOTICE, "Batman daemon spawned.");

	while (1){

		printf("In the Daemon loop.\n");
		update_data(power_modes, batman_work_dir);
		printf("Data updated.\n");
		sleep(INTERVAL);
		break;
	}


	/*  Closing log */
	syslog(LOG_NOTICE, "Batman daemon terminated.");
	closelog();
}