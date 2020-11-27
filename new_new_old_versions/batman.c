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

	// parent id of calling process
	pid_t ppid = getppid();

	pid_t pid;		// child process id
	pid_t sid;		// child session id

	/* fork off the parent process */
	pid = fork();

	if ( pid < 0)
		error("[-] Failed to fork parent");

	/* upon success, let parent terminate */
	if ( pid > 0)
		exit(0);		// terminates and exits from the parent


	/* Child process now becomes session leader */
	sid = setsid();
	if ( sid < 0 )
		error("[-] Failed to make child process the session leader");

	/*
	 * Catch, ignore and handle signals
	 * Implement a working signal handler
	 * Closing stdin, stdout and stderr open file descriptors
	 *
	 */
	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);


	// fork off te second time
	pid = fork();

	if ( pid < 0 )
		error("[-] Failed to perform a second fork");

	// let parent terminate
	if ( pid > 0 )
		exit(0);

	// set new file permissions. Set daemon's file mode creation mask
	umask(0);

	// change directory to root or a working directory
	chdir("/");

	/* close all open file descriptors */
	int x;
	for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--){
		// printf("sysconf: %s\n", x);
		close(x);
	}


	/* Open log files for logging */
	openlog("batmand", LOG_PID, LOG_DAEMON);
}


int main(int argc, char *argv[]){

	/* Parsing user input arguments */


	/* START BATMAN DAEMON */
	batman_daemon();


	/* Spawn daemon activities */
	syslog(LOG_NOTICE, "Batman daemon spawned.");

	while (1){

		syslog(LOG_NOTICE, "Batman daemon example started.");
		sleep(INTERVAL);
		break;
	}


	/*  Closing log */
	syslog(LOG_NOTICE, "Batman daemon terminated.");
	closelog();

	return 0;
}