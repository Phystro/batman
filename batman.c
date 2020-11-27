/*
 * Name:	Batman
 * Version:	0.0.1
 * Author:	Anthony Karoki
 * Description:	Energy consumption, power usage and battery moniroring program for Linux systems
 * Copyright (C) Anthony Karoki - Eccentric Tensor Labs
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <getopt.h>			// Parsing command-line arguments
#include <syslog.h>
#include <signal.h>
#include <sys/stat.h>			// Checking if directory exists
#include <sys/types.h>

#include "batman.h"



/*
 * Structure for storing command-line options and arguments
 */
struct globalArgs_t {
	int info;		// --info/-i option; quick battery info/intel
	int stats;		// --stats/-s option; energy stats; battery or AC Mains
	int start_daemon;	// --start option; starts the daemon if daemon was stopped
	int stop_daemon;	// --stop option; stops the daemon if daemon was running
} globalArgs;


static const char *optString = ":ish?";


/*
 * longOpts array to hold info about long options required
 */
static const struct option longOpts[] = {
	{ "info", no_argument, NULL, 'i' },
	{ "stats", no_argument, NULL, 's' },
	{ "start_daemon", no_argument, NULL, 0 },
	{ "stop_daemon", no_argument, NULL, 0 },
	{ "help", no_argument, NULL, 'h' },
	{ NULL, no_argument, NULL, 0 }
};



int main( int argc, char *argv[] ){
	
	/*
	 * -------------------------------------------------------------------------------------
	 * Parsing user input arguments
	 * -------------------------------------------------------------------------------------
	 */

	/* Initialize globalArgs struct before parsing */
	globalArgs.info = 0;						// false
	globalArgs.stats = 0;
	globalArgs.start_daemon = 0;
	globalArgs.stop_daemon = 0;

	int options, long_index;
	options = getopt_long( argc, argv, optString, longOpts, &long_index );

	while ( options != -1 ){
	
		switch( options ){
		
			case 'i':
				globalArgs.info = 1;			// true
				/* Function call here. Display Info */
				display_info();
				break;

			case 's':
				globalArgs.stats = 1;
				/* Function call here. Display Stats */
				display_stats();
				break;

			case 'h':					// fall through is intentional. Manages
			case '?':					// ':' no value and '?' unknown option errors
				display_usage();
				break;

			case 0:						// long option without a short argument
				if ( strncmp( "start_daemon", longOpts[long_index].name, 12 ) == 0 ){
					globalArgs.start_daemon = 1;
					/* Function call here */
					globalArgs.stop_daemon = 0;
				}
				
				else if ( strncmp( "stop_daemon", longOpts[long_index].name, 11 ) == 0 ){
					globalArgs.stop_daemon = 1;
					/* Function call here */
					globalArgs.start_daemon = 0;
				}
				
				else{
					display_usage();
				}

				break;

			default:
				break;
		}

		options = getopt_long( argc, argv, optString, longOpts, &long_index );
	}

	return EXIT_SUCCESS;
}

