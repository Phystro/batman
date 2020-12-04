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
	// pid_t ppid;
	int has_tty;

	pid = get_proc_id_by_name( "batman" );
	// ppid = get_ppid_by_pid( pid );
	has_tty = pid_has_tty( pid );

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
			printf("%s[-]%s %sNo instance of the batman daemon is currently running%s\n", redb(), resetc(), red(), resetc() );
		}
	}

	else if ( pid > 0 ){
		// such a process already exists

		if ( ( start_daemon == 1 ) && ( stop_daemon == 0 ) ){
			if ( has_tty == 0 ){
				// daemon process exists and is running
				printf("%s[-]%s %sAn instance of the batman daemon is already running%s\n", yellowb(), resetc(), yellow(), resetc() );
			}
			else if ( has_tty == 1 ) {
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
			if ( has_tty == 0 ){
				// terminate the daemon
				printf("%s[+]%s %sTerminating batman daemon. Detective Mode OFF%s\n", greenb(), resetc(), green(), resetc() );
				kill( pid, 15 );			// terminate using SIG 15
				syslog( LOG_NOTICE, "Batman daemon terminated [ Detective mode OFF ]" );
				closelog();
			} 
			else if ( has_tty == 1 ){
				perror("Unethical to kill a name-sake, don't you think!");
			}
		}
	}
}


void batman_daemon(){

	// get parent pid of calling process
	// pid_t ppid = getppid();

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
	umask( 022 );						// folders open as (777 - 022 = 755)rwxr_xr_x files as (666 - 022 = 644)rw_r__r__

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
	char *VAR_WORK_PATH = malloc( BUFFSIZE );
	strcpy( VAR_WORK_PATH, get_home_dir() );
	strcat( VAR_WORK_PATH,  "/");
	strcat( VAR_WORK_PATH, VAR_WORK_DIR );
	struct stat st = {0};

	if ( stat( VAR_WORK_PATH, &st ) == -1 ){
		mkdir( VAR_WORK_PATH, 0755 );					// rwxr_xr_x permissions
	}

	// create the /var/lib/power_mode directories
	char *power_mode_dir = malloc( BUFFSIZE );
	
	char *power_modes[BUFFSIZE];
	get_power_modes( power_modes );

	for ( int i = 0; ; i++ ){
	
		if ( power_modes[i] == NULL )
			break;

		strcpy( power_mode_dir, VAR_WORK_PATH );
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
		PS_CHARGE_FULL_DESIGN, PS_CHARGE_FULL, PS_CHARGE_NOW, PS_CHARGE_NOW,
		PS_VOLTAGE_MIN_DESIGN, PS_VOLTAGE_NOW
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

			// writing data from double array into data files in ~/.batman
			for ( int k = 0; k < 6; k++ ){
				
				if ( write_data_files[k] == NULL )
					break;

				// don't record in zeros into the data
				if ( base_data[k] == 0 )
					continue;

				strcpy( write_data_filename, VAR_WORK_PATH );
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

				// Don't record in zeros into the data
				if ( base_data[z] == 0 )
					continue;
				
				strcpy( write_data_filename, VAR_WORK_PATH );
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

			/*
			 * Record into into stats file at root of batman for notifications to read
			 */
			char *stats_filename = malloc(BUFFSIZE);
			strcpy( stats_filename, VAR_WORK_PATH );
			strcat( stats_filename, "/stats" );
			double bat_health = 100 * calc_ratio( base_data[1], base_data[0] );
			double worn_out = 100 - bat_health;
			double last_capacity = 100.0 * calc_ratio( base_data[2], base_data[1] );
			// char buf[BUFFSIZE];
			// sprintf( buf, "%d\t", last_capacity );
			// write_file_line( stats_filename, base_data[2] );
			// sprintf( buf, "%d\t", bat_health );
			// append_file_line( stats_filename, 100.0*calc_ratio( base_data[1], base_data[0] ) );
			// sprintf( buf, "%d\t", worn_out );
			// append_file_line( stats_filename, 100.0 - bat_health );

			free( stats_filename );			

		}
		
		/*
		 * A while loop that loops 60 times, equivalent to 60 seconds
		 * Checks the last battery charge capacity, then sleeps for 1 second
		 * Depending on last battery charge cpacity, it calls the notifications function to notify the user
		 */
		
		get_power_modes( power_modes );

		int index = 0;
		char *capacity_filename = malloc( BUFFSIZE );
		char *status_filename = malloc( BUFFSIZE );
		char *status_buff = malloc(BUFFSIZE);
		char *capacity_buff = malloc(BUFFSIZE);

		while ( index < INTERVAL ){
			 // battery status check code
			for ( int i = 0; ; i++ ){
				
				if ( power_modes[i] == NULL )
					break;
				
				// reading into status filename
				strcpy( status_filename, POWER_SUPPLY_DIR );
				strcat( status_filename, power_modes[i] );
				strcat( status_filename, "/" );
				strcat( status_filename, PS_STATUS );

				// reading into capacity filename
				strcpy( capacity_filename, POWER_SUPPLY_DIR );
				strcat( capacity_filename, power_modes[i] );
				strcat( capacity_filename, "/" );
				strcat( capacity_filename, PS_CAPACITY );	

				read_file_line( status_filename, status_buff );
				read_file_line( capacity_filename, capacity_buff );
				
				if ( strncmp( status_buff, "non", 3 ) != 0  && strncmp( capacity_buff, "non", 3 ) != 0 ){
					
					if ( strcmp( status_buff, toggle ) != 0 ){
						strcpy( toggle, status_buff );
						get_power_modes( power_modes );
						/*
						 * Adjust the icon images and messages to be displayed on notification
						 * 	depending on reason i.e. state situation
						 * 	e.g. charging, unknown, full...e.t.c.
						 */	
						int cap = atoi( capacity_buff );
						if ( strncmp( toggle, "Charging", 8 ) == 0 ){
							// different icons for different charging levels
							if ( cap <= 10 )
							{
								const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-charge-empty-battery.png";
								display_notifications( power_modes[i], toggle, 1, NULL, icon );	// set urgency level to normal = 1
							} 
							else if ( cap <= 20 && cap > 10 )
							{
								const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-charging-empty-battery.png";
								display_notifications( power_modes[i], toggle, 1, NULL, icon );	// set urgency level to normal = 1
							}
							else if ( cap < 50 && cap > 20 )
							{
								const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-charging-low-battery.png";
								display_notifications( power_modes[i], toggle, 1, NULL, icon );	// set urgency level to normal = 1
							}
							else if ( cap >= 50 && cap <= 75 )
							{
								const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-medium-charging-battery.png";
								display_notifications( power_modes[i], toggle, 1, NULL, icon );	// set urgency level to normal = 1
							}
							else if ( cap > 75 && cap < 100)
							{
								const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-battery.png";
								display_notifications( power_modes[i], toggle, 1, NULL, icon );	// set urgency level to normal = 1
							}		
						}

						if ( strncmp( toggle, "Discharging", 11 ) == 0 ){
							// different icons for different discharging levels
							if ( cap <= 8 )
							{
								char *caution = "CRITICALLY LOW";
								const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-warning-battery.png";
								display_notifications( power_modes[i], toggle, 1, caution, icon );	// set urgency level to normal = 1
							}
							else if ( cap > 8 && cap <= 19 )
							{
								char *caution = "LOW";
								const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-nearly-empty-battery.png";
								display_notifications( power_modes[i], toggle, 1, caution, icon );	// set urgency level to normal = 1
							}
							else if ( cap > 19 && cap < 50 )
							{
								const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-low-battery.png";
								display_notifications( power_modes[i], toggle, 1, NULL, icon );	// set urgency level to normal = 1
							}
							else if ( cap >= 50 && cap < 75 )
							{
								const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-battery-medium-level.png";
								display_notifications( power_modes[i], toggle, 1, NULL, icon );	// set urgency level to normal = 1
							}
							else if ( cap >= 75 && cap < 100 )
							{
								const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-charged-discharging-battery.png";
								display_notifications( power_modes[i], toggle, 1, NULL, icon );	// set urgency level to normal = 1
							}
							else if ( cap == 100 )
							{
								char *caution = "FULLY CHARGED";
								const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-full-battery.png";
								display_notifications( power_modes[i], toggle, 1, caution, icon );	// set urgency level to normal = 1
							}
						}

						if ( strncmp( toggle, "Unknown", 7 ) == 0 ){
							const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-battery-unknown.png";
							char *caution = "UNDEFINED BATTERY STATE";
							display_notifications( power_modes[i], toggle, 0, caution, icon );	// set urgency level to normal = 0
						}

						if ( strncmp( toggle, "Full", 4 ) == 0 ){
							const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-full-battery.png";
							char *caution = "FULLY CHARGED.\nUNPLUG FROM A/C MAINS";
							display_notifications( power_modes[i], toggle, 2, caution, icon );	// urgency level critical = 2
						}

						if ( cap < 12 ){
							const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-nearly-empty-battery.png";
							char *caution = "LOW CHARGE";
							display_notifications( power_modes[i], toggle, 1, caution, icon );	// set urgency level to normal = 1
						}
					}else{
						continue;
					}
				
				} else{
					continue;
				}
			}

			memset( status_buff, 0, BUFFSIZE );
			memset( capacity_buff, 0, BUFFSIZE );

			index++;
			sleep( 1 );
			get_power_modes( power_modes );
		}

		free( capacity_buff );
		free( status_buff );
		free( capacity_filename );
		free( status_filename );
	}

	free( VAR_WORK_PATH );
	free( read_data_filename );
	free( write_data_filename );
	free( data_buffer );
}


