/*
 * Name:	Batman
 * Version:	0.1.1
 * Author:	Anthony Karoki
 * Description:	Battery and AC Technology Monitoring and Analysis Notification program for Linux systems
 * Copyright (C) Anthony Karoki - Eccentric Tensor Labs
 *
 */

#include "../include/batman.h"	


void display_usage( char *prog_name ){
	printf( "\n%s v0.1.0\n", prog_name );
	printf( "Battery AC Technology Monitoring Analysis and Notifications.\n" );
	printf( "\n" );
	printf( "USAGE:\n" );
	printf( "\t%s [OPTIONS]...\n", prog_name );
	printf( "\n" );
	printf( "OPTIONS:\n" );
	printf( "\t-i, --info\t\tDisplay summarised battery and/or AC Mains information.\n" );
	printf( "\t-s, --stats\t\tDisaply battery or AC Mains stats information.\n" );
	printf( "\n" );
}


/*
 * Structure for storing command-line options and arguments
 */
struct globalArgs_t {
	int info;		// --info/-i option; quick battery info/intel
	int stats;		// --stats/-s option; energy stats; battery or AC Mains
	int events;		// --events/-e option; starts battery events monitoring to spawn notifications. To be used by systemd calls only.
} globalArgs;


static const char *optString = ":ish?";


/*
 * longOpts array to hold info about long options required
 */
static const struct option longOpts[] = {
	{ "info", no_argument, NULL, 'i' },
	{ "stats", no_argument, NULL, 's' },
	{ "events", no_argument, NULL, 0 },
	{ "help", no_argument, NULL, 'h' },
	{ NULL, no_argument, NULL, 0 },
	{ NULL, 0, 0, 0 }
};



int main( int argc, char *argv[] ){
	
	/* Initialize globalArgs struct before parsing */
	globalArgs.info = 0;						// false
	globalArgs.stats = 0;

	/* 
	 * ------------------------------------------------------------------------------------
	 * if no arguments are provided, prcoeed to check status of batman daemon, then exit
	 * ------------------------------------------------------------------------------------
	 */
	
	if ( argc == 1 ){
		uint16_t pid;
		uint16_t verify_cmd;

		pid = get_proc_id_by_name( "batman" );
		verify_cmd = verify_cmdline( pid, "batman", 6 );

		if ( pid < 0 )
			monitor_events();

		else if ( pid > 0 ){

			if ( verify_cmd == 1 ){
				printf("%s[-]%s %sAn instance of the batman battery events monitor is already running as PID: %d%s\n", yellowb(), resetc(), yellow(), pid, resetc() );
				exit( EXIT_SUCCESS );
			}
			else if ( verify_cmd == 0 )
				monitor_events();
		}
	}

	
	/*
	 * -------------------------------------------------------------------------------------
	 * Parsing user input arguments
	 * -------------------------------------------------------------------------------------
	 */
	
	int options, long_index;
	options = getopt_long( argc, argv, optString, longOpts, &long_index );

	while ( options != -1 ){
	
		switch( options ){
		
			case 'i':
				globalArgs.info = 1;			// true
				display_info();
				break;

			case 's':
				globalArgs.stats = 1;
				display_stats();
				break;

			case 'h':					// fall through is intentional. Manages
			case '?':					// ':' no value and '?' unknown option errors
				display_usage( argv[0] );
				break;

			case 0:						// long option without a short argument
				if ( strncmp( "events", longOpts[long_index].name, 6 ) == 0 ){
					/*
					 * TODO: Implement process control, prevent launching of several instances
					 */
					globalArgs.events = 1;
					monitor_events();
				}

			default:
				display_usage( argv[0] );
				exit( EXIT_SUCCESS );
				break;
		}

		options = getopt_long( argc, argv, optString, longOpts, &long_index );
	}

	return EXIT_SUCCESS;
}

