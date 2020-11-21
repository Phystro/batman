/*
 * Name:	Batman
 * Version:	0.0.1
 * Author:	Anthony Karoki
 * Alias:	PhyTensor
 * Description:	Energy consumption, power usage and battery monitoring program for Linux systems.
 * Copyright (C) Eccentric Tensor Labs (2020)
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>			// checking if directory exists
#include <sys/types.h>
#include <syslog.h>
#include <signal.h>

#include "batman.h"

static void monitor_daemon(char *workDir){

	/* Declare child process id */
	pid_t pid;
	/* Declare child session id */
	pid_t sid;

	/* Fork off parent process */
	pid = fork();
	if ( pid < 0 )
		perror("[-] Failed to fork off parent process");
	if ( pid > 0 ){
		printf("[+] Parent process terminated successfully\n");
		exit(0);
	}

	/* Make child process the session leader */
	sid = setsid();
	if ( sid < 0 )
		perror("[-] Failed to make child process the session leader");

	/*
	 * Catch, ignore and handle signals
	 * Signal handling.
	 * Closing stdin, stdout and stderr
	 */
	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	/* Set new file permissions. Set daemon's file mode creation mask */
	umask(0);

	/* Change the working directory to that of the programs working directory */
	chdir(workDir);

	/* Open the log file for logging */
	openlog("batmand", LOG_PID, LOG_DAEMON);

}

int main(int argc, char *argv[]){

	// parsing user input

	
	// if first time run, create a home working directory
	char *home = getHomeDir();
	char *workDir = strcat(home, "/.batman/");
	struct stat st = {0};
	if ( stat(workDir, &st) == -1){
		mkdir(workDir, 0644);		// rw_r__r__ permissions
	}

	// START BATMAN DAEMON
	monitor_daemon(workDir);

	/* Closing log */
	syslog(LOG_NOTICE, "Batman daemon terminated.");
	closelog();

	return 0;
}
