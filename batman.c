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
	 * Implement Signal handling code here.
	 * Closing stdin, stdout and stderr open file descriptors
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

	/* parsing user input arguments */

	
	// If first time run, create a home working directory
	char *home = getHomeDir();
	char *workDir = malloc(BUFFSIZE);
	strcpy(workDir, home);
	strcat(workDir, "/.batman/");
	struct stat st = {0};
	if ( stat(workDir, &st) == -1){
		mkdir(workDir, 0644);		// rw_r__r__ permissions
	}

	// If first time run, create power supply directories on the home working directory
	char *tmp = malloc(BUFFSIZE);
	char *power_supplies[BUFFSIZE];
	get_power_supplies(power_supplies);
	for (int i = 0; ; i++){
		if ( power_supplies[i] == NULL)
			break;
		memset(tmp, 0, sizeof(workDir));		// clear buffer data with zeros
		// printf("%d %s\n", i, power_supplies[i] );

		strcpy(tmp, workDir);
		strcat(tmp, power_supplies[i]);
		// printf("tmp %s\n", tmp);
		if ( stat( tmp , &st) == -1 )
			mkdir(tmp , 0644);
		// printf("workdir %s\n", workDir);
		// printf("home %s\n", home);
	}
	free(tmp);


	// START BATMAN DAEMON
	monitor_daemon(workDir);

	/* Spawn Daemon activities */
	syslog(LOG_NOTICE, "Batman daemon spawned.");
	while (1){
	
		sleep(INTERVAL);
		break;
	}


	/* Closing log */
	syslog(LOG_NOTICE, "Batman daemon terminated.");
	closelog();

	return 0;
}
