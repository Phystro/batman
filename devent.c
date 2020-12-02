/*
 * Daemon Events file
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "batman.h"


void task_master( int start_daemon, int stop_daemon ){

	pid_t pid;
	pid_t ppid; 

	pid = get_proc_id_by_name( "batman" );
	ppid = get_ppid_by_pid( pid );

	if ( pid < 0 ){
		// process doesn't exist

		if ( ( start_daemon == 1 ) && ( stop_daemon == 0 ) ){
			// spawn the daemon
			batman_daemon();
			syslog( LOG_NOTICE, "Batman daemon spawned" );
			// spawn daemon activities / detective mode
			syslog( LOG_NOTICE, "Batman daemon activities started [ Detective mode ON ]" );
			batman_daemon_detective();
		}
		
		else if ( ( start_daemon == 0 ) && ( stop_daemon == 1 ) ){
			printf("%s[-]%s %sNo instance of batman is currently running%s\n", redb(), resetc(), red(), resetc() );
		}
	}

	else if ( pid > 0 ){
		// such a process already exists

		if ( ( start_daemon == 1 ) && ( stop_daemon == 0 ) ){
			if ( ppid == 1 ){
				printf("%s[-]%s %sAn instance of batman is already running%s\n", yellowb(), resetc(), yellow(), resetc() );
			}
			else if ( ppid != 1 ) {
				printf("%s[+]%s %sSpawning the batman. Activating Detective Mode ON%s\n", greenb(), resetc(), green(), resetc() );
				// spawn the daemon
				batman_daemon();
				syslog( LOG_NOTICE, "Batman daemon spawned" );
				// spawn daemon activities / detective mode
				syslog( LOG_NOTICE, "Batman daemon activities started [ Detective mode ON ]" );
				batman_daemon_detective();
			}
		}
		
		else if ( ( start_daemon == 0 ) && ( stop_daemon == 1 ) ){
			if ( ppid == 1 ){
				// terminate the daemon
				printf("%s[+]%s %sTerminating batman daemon. Detective Mode OFF%s\n", greenb(), resetc(), green(), resetc() );
				kill( pid, 15 );			// terminate using SIG 15
				syslog( LOG_NOTICE, "Batman daemon terminated [ Detective mode OFF ]" );
				closelog();
			} 
			else if ( ppid != 1 ){
				perror("Unethical to kill a name-sake, don't you think!");
			}
		}
	}
}


void batman_daemon(){

	// get parent pid of calling process
	pid_t ppid = getppid();

	pid_t pid;					// program's process id
	pid_t sid;					// program's session id
	
	/* Fork() off the current program process */
	pid = fork();					// returns child process id

	if ( pid < 0 )
		error("[-] Failed to fork off parent");

	/* Upon success fork(), terminate parent/ current program */
	if ( pid > 0 )
		exit( EXIT_SUCCESS );

	/* Make current child process, the session leader */
	sid = setsid();
	if ( sid < 0 )
		error("[-] Failed to make child process the session leader");

	/*
	 * Catch, ignore and handle signals
	 * Implement a working signal handler function
	 * Closing down stdin, stdout and stderr open file descriptors
	 */
	signal( SIGCHLD, SIG_IGN );
	signal( SIGHUP, SIG_IGN );
	close( STDIN_FILENO );
	close( STDOUT_FILENO );
	close( STDERR_FILENO );

	// fork off the second time
	// pid = fork();					// new pid, child process

	/*
	 * Set new file permissions
	 * Set daemon's file mode creation mask
	 */
	umask( 0 );

	// Change program directory to its working directory
	chdir( "/" );

	/* Close all open file descriptors */
	int x;
	for ( x = sysconf( _SC_OPEN_MAX ); x >= 0; x-- ){
		close( x );
	}

	/* Open log files for logging daemon */
	openlog("batman", LOG_PID, LOG_DAEMON);

}


/* Spawn Batman daemon activities */
void batman_daemon_detective(){

	char *toggle = malloc( BUFFSIZE );					// for monitoring battery status > charging/discharging states
	strcpy( toggle, "Charging" );

	/*
	 * For the first time program run, create home working directory in /var/lib/
	 * 	and the power modes directory
	 */
	char *var_work_dir = malloc( BUFFSIZE );
	strcpy( var_work_dir,  "/var/lib/");
	strcat( var_work_dir, "batman/" );
	struct stat st = {0};

	if ( stat( var_work_dir, &st ) == -1 ){
		mkdir( var_work_dir, 0755 );					// rwxr_xr_x permissions
	}

	// create the /var/lib/power_mode directories
	char *power_mode_dir = malloc( BUFFSIZE );
	
	char *power_modes[BUFFSIZE];
	get_power_modes( power_modes );

	for ( int i = 0; ; i++ ){
	
		if ( power_modes[i] == NULL )
			break;

		strcpy( power_mode_dir, var_work_dir );
		strcat( power_mode_dir, power_modes[i] );

		if ( stat( power_mode_dir, &st ) == -1 ){
			mkdir( power_mode_dir, 0755 );
		}
	}
	
	free( power_mode_dir );


	/*
	 * Data collection
	 */
	const char *read_data_files[6] = 
	{
		"charge_full_design", "charge_full", "charge_now", "current_now",
		"voltage_min_design", "voltage_now"
	};

	const char *write_data_files[6] = 
	{
		"Full Design Charge Capacity", "Full Usage Charge Capacity", "Present Charge Capacity",
		"Present Current", "Minimum Design Voltage", "Present Voltage"
	};

	const char *derived_data_files[2] = 
	{
		"Power Consumption", "Design Charge Capacity"
	};

	double base_data[6] = {0};

	char *read_data_filename = malloc( BUFFSIZE );
	char *write_data_filename = malloc( BUFFSIZE );
	char *data_buffer = malloc( BUFFSIZE );
	
	while ( 1 ){

		for ( int i = 0; ; i++ ){

			get_power_modes( power_modes );

			// printf("modes %d: %s\n", i, power_modes[i]);

			memset( base_data, 0, 6 * sizeof (double) );
		
			if ( power_modes[i] == NULL )
				break;

			// reading data files from power supply and storing in a double array
			for ( int j = 0; j < 6; j++ ){
			
				if ( read_data_files[j] == NULL )
					break;

				strcpy( read_data_filename, POWER_SUPPLY_DIR );
				strcat( read_data_filename, power_modes[i] );
				strcat( read_data_filename, "/" );
				strcat( read_data_filename, read_data_files[j] );

				read_file_line( read_data_filename, data_buffer );

				if ( strncmp( data_buffer, "non", 3 ) != 0 ){
				
					if ( j < 4 ){
						base_data[j] = (int) ( strtod( data_buffer, NULL )/1000.0 ) ;
					} else{
						base_data[j] = strtod( data_buffer, NULL ) / 1000000.0;
					}
				} else{
					continue;
				}
				
				memset( data_buffer, 0, BUFFSIZE );
			}

			// writing data from double array into data files in /var/lib/batman
			for ( int k = 0; k < 6; k++ ){
				
				if ( write_data_files[k] == NULL )
					break;

				if ( base_data[k] == 0 )
					continue;

				strcpy( write_data_filename, var_work_dir );
				strcat( write_data_filename, power_modes[i] );
				strcat( write_data_filename, "/" );
				strcat( write_data_filename,  write_data_files[k]);

				if ( (k == 0) || (k == 4) )
					write_file_line( write_data_filename, base_data[k] );
				else
					append_file_line( write_data_filename, base_data[k] );
			}

			// writing derived data, useful calculated data
			for ( int z = 0; z < 2; z++ ){
			
				if ( derived_data_files[z] == NULL )
					break;

				if ( base_data[z] == 0 )
					break;
				
				strcpy( write_data_filename, var_work_dir );
				strcat( write_data_filename, power_modes[i] );
				strcat( write_data_filename, "/" );
				strcat( write_data_filename, derived_data_files[z] );

				if ( z == 0 ){
					// power consumption
					append_file_line( write_data_filename, calc_product( base_data[3]/1000, base_data[5] ) );
				}
				else if ( z == 1 ){
					// design charge capacity
					append_file_line( write_data_filename, 100*calc_ratio( base_data[1], base_data[0] ) );

				}
			}
		}

		syslog( LOG_NOTICE, "Batman daemon example started" );
		// sleep( INTERVAL );

		
		/*
		 * A while loop that loops 60 times, equivalent to 60 seconds
		 * Checks the last battery charge capacity, then sleeps for 1 second
		 * Depending on last battery charge cpacity, it calls the notifications function to notify the user
		 */
		
		get_power_modes( power_modes );

		int index = 0;
		while ( index < INTERVAL ){
		//for ( index; index < INTERVAL; index++ ){	
			 // battery status check code
			for ( int i = 0; ; i++ ){
				
				if ( power_modes[i] == NULL )
					break;

				memset( data_buffer, 0, BUFFSIZE );
				
				strcpy( read_data_filename, POWER_SUPPLY_DIR );
				strcat( read_data_filename, power_modes[i] );
				strcat( read_data_filename, "/" );
				strcat( read_data_filename, "status" );

				read_file_line( read_data_filename, data_buffer );
				
				if ( strncmp( data_buffer, "non", 3 ) != 0 ){
					syslog(LOG_NOTICE, "batman, file found");
					
					if ( strcmp( data_buffer, toggle ) != 0 ){
						strcpy( toggle, data_buffer );
						get_power_modes( power_modes );
						/*
						 * Adjust the icon images and messages to be displayed on notification
						 * 	depending on reason i.e. state situation
						 * 	e.g. charging, unknown, full...e.t.c.
						 */	
						if ( strncmp( toggle, "Charging", 8 ) == 0 ){
							const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-medium-charging-battery.png";
							display_notifications( power_modes[i], toggle, 1, NULL, icon );	// set urgency level to normal = 1
						}

						if ( strncmp( toggle, "Discharging", 11 ) == 0 ){
							const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-charged-discharging-battery.png";
							display_notifications( power_modes[i], toggle, 1, NULL, icon );	// set urgency level to normal = 1
						}

						if ( strncmp( toggle, "Unknown", 7 ) == 0 ){
							const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-battery-unknown.png";
							char *caution = "UNDEFINED BATTERY STATE";
							display_notifications( power_modes[i], toggle, 0, caution, icon );	// set urgency level to normal = 0
						}

						if ( strncmp( toggle, "Full", 4 ) == 0 ){
							const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-full-battery.png";
							char *caution = "BATTERY FULLY CHARGED. UNPLUG FROM A/C MAINS";
							display_notifications( power_modes[i], toggle, 2, caution, icon );	// urgency level critical = 2
						}

						if( strncmp( toggle, "Low", 3 ) ==0 ){
							const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-low-battery.png";
							char *caution = "LOW CHARGE. CONSIDER CHARGING";
							display_notifications( power_modes[i], toggle, 2, caution, icon );	// urgency level critical = 1
						}
					}else{
						continue;
					}
				
				} else{
					//syslog(LOG_NOTICE, "batman, no such file");
					continue;
				}
	
				//syslog( LOG_NOTICE, "battery status check code" );
			}

			memset( data_buffer, 0, BUFFSIZE );
			//syslog( LOG_NOTICE, "exited battery status check code" );


			// battery last capacity check
/*			for ( int i = 0; ; i++ ){

				strcpy( read_data_filename, POWER_SUPPLY_DIR );
				strcat( read_data_filename, power_modes[i] );
				strcat( read_data_filename, "/" );
				strcat( read_data_filename, "capacity" );

				read_file_line( read_data_filename, data_buffer );

				int bat_capacity = atoi( data_buffer );

				if ( bat_capacity == 100 ){
					display_notifications( "FULL CAPACITY" );
				}

				memset( data_buffer, 0, BUFFSIZE );
			}

*/

			index++;
			sleep( 1 );
			get_power_modes( power_modes );
		}
	
		// sleep(5);

	}

	free( var_work_dir );
	free( read_data_filename );
	free( write_data_filename );
	free( data_buffer );
}


