#include "../include/batman.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>


void batman_daemon(){

	pid_t pid;					// program's process id
	pid_t sid;					// program's session id
	char *buf_pid = malloc( BUFFSIZE );		// store program process id as a char array

	/* Fork() off the current program process */
	pid = fork();					// on success returns; child pid to the parent, 0 to child

	/* Upon success fork(), terminate parent/ current program */
	if ( pid < 0 )
		errorlog("[-] Failed to fork off parent");
	
	else if ( pid > 0 )
		exit( EXIT_SUCCESS );

	/* Create PID file to store PID of daemon for use by systemd service configuration files. */
	sprintf( buf_pid, "%d", getpid() );
	write_file_line_as_char( "/var/run/batmand.pid",  buf_pid );
	free( buf_pid );


	/* Make current child process, the session leader */
	sid = setsid();
	if ( sid < 0 )
		errorlog("[-] Failed to make child process the session leader");


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

	/* Change program directory to its working directory */
	chdir( "/" );

	/* Close all open file descriptors */
	int x;
	for ( x = sysconf( _SC_OPEN_MAX ); x >= 0; x-- ){
		close( x );
	}

	/* Open log files for logging daemon */
	openlog("batmand", LOG_PID, LOG_DAEMON);

}


void batman_daemon_detective(){

	syslog( LOG_NOTICE, "Spawned batmand daemon. Batmand running..." );

//-------------------------------------------------------------------------------------------------------------------------------------------

	uint8_t toggle_cap = 0;

	/*
	 * ------------------------------------------------------------------------------------------------------------------------
	 * For the first time program run, create home working directory in /var/lib/
	 * 	and the power modes directory
	 * -------------------------------------------------------------------------------------------------------------------------
	 */
	char *VAR_WORK_PATH = malloc( BUFFSIZE );
	strcpy( VAR_WORK_PATH, "/var/lib/" );
	strcat( VAR_WORK_PATH, VAR_WORK_DIR );
	struct stat st = {0};

	if ( stat( VAR_WORK_PATH, &st ) == -1 ){
		mkdir( VAR_WORK_PATH, 0755 );					// rwxr_xr_x permissions
	}

	/* create the /$HOME/power_mode directories */
	char *power_mode_dir = malloc( BUFFSIZE );
	
	char *power_modes[BUFFSIZE];
	get_power_modes( power_modes );

	for ( uint8_t i = 0; ; i++ ){
	
		if ( power_modes[i] == NULL )
			break;

		strcpy( power_mode_dir, VAR_WORK_PATH );
		strcat( power_mode_dir, power_modes[i] );

		if ( stat( power_mode_dir, &st ) == -1 ){
			mkdir( power_mode_dir, 0755 );
		}
	}
	
	free( power_mode_dir );


	/* Data collection files */
	const char *read_data_files[6] = 
	{
		PS_CHARGE_FULL_DESIGN, PS_CHARGE_FULL, PS_CHARGE_NOW, PS_CHARGE_NOW,
		PS_VOLTAGE_MIN_DESIGN, PS_VOLTAGE_NOW
	};

	/* Write Data files */
	const char *write_data_files[6] = 
	{
		"Full Design Charge Capacity", "Full Usage Charge Capacity", "Present Charge Capacity",
		"Present Current", "Minimum Design Voltage", "Present Voltage"
	};

	/* Dervied Data Files */
	const char *derived_data_files[2] = 
	{
		"Power Consumption", "Design Charge Capacity"
	};

	double base_data[6] = {0};

	char *read_data_filename = malloc( BUFFSIZE );
	char *write_data_filename = malloc( BUFFSIZE );
	char *data_buffer = malloc( BUFFSIZE );


	while ( 1 ){
		/*
		 * A while loop that loops 60 times, equivalent to 60 seconds
		 * Checks the last battery charge capacity, then sleeps for 1 second
		 * Depending on last battery charge cpacity, it calls the notifications function to notify the user
		 */

		for ( uint8_t i = 0; ; i++ ){

			get_power_modes( power_modes );

			memset( base_data, 0, 6 * sizeof (double) );
		
			if ( power_modes[i] == NULL )
				break;

			/* reading data files from power supply and storing in a double array */
			for ( uint8_t j = 0; j < 6; j++ ){
			
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

			/* writing data from double array into data files in ~/.batman */
			for ( uint8_t k = 0; k < 6; k++ ){
				
				if ( write_data_files[k] == NULL )
					break;

				/* don't record in zeros into the data */
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

			/* writing derived data, useful calculated data */
			for ( uint8_t z = 0; z < 2; z++ ){
			
				if ( derived_data_files[z] == NULL )
					break;

				/* Don't record in zeros into the data */
				if ( base_data[z] == 0 )
					continue;
				
				strcpy( write_data_filename, VAR_WORK_PATH );
				strcat( write_data_filename, power_modes[i] );
				strcat( write_data_filename, "/" );
				strcat( write_data_filename, derived_data_files[z] );

				if ( z == 0 ){
					/* power consumption */
					append_file_line( write_data_filename, calc_product( base_data[3]/1000, base_data[5] ) );
				}
				else if ( z == 1 ){
					/* design charge capacity */
					append_file_line( write_data_filename, 100*calc_ratio( base_data[1], base_data[0] ) );

				}
			}	

		}


		get_power_modes( power_modes );

		uint8_t index = 0;									// counter

		char *filename_charge_full_design = malloc( BUFFSIZE );
		char *filename_charge_full = malloc( BUFFSIZE );
		char *filename_capacity = malloc( BUFFSIZE );

		char *buff_charge_full_design = malloc( BUFFSIZE );
		char *buff_charge_full = malloc( BUFFSIZE );
		char *buff_capacity = malloc(BUFFSIZE);


		while( index < ( INTERVAL * 2 ) ){

			/* repeat loop only after 120s, data collected and recorded after every 120 seconds */
			/* battery status check code */
			
			for ( uint8_t i = 0; ; i++ ){

				if ( power_modes[i] == NULL )
					break;		

				/* reading into capacity filename */
				strcpy( filename_capacity, POWER_SUPPLY_DIR );
				strcat( filename_capacity, power_modes[i] );
				strcat( filename_capacity, "/" );
				strcat( filename_capacity, PS_CAPACITY );

				/* reading into the charge_full_design file */
				strcpy( filename_charge_full_design, POWER_SUPPLY_DIR );
				strcat( filename_charge_full_design, power_modes[i] );
				strcat( filename_charge_full_design, "/" );
				strcat( filename_charge_full_design, PS_CHARGE_FULL_DESIGN );

				/* reading into the charge_full file */
				strcpy( filename_charge_full, POWER_SUPPLY_DIR );
				strcat( filename_charge_full, power_modes[i] );
				strcat( filename_charge_full, "/" );
				strcat( filename_charge_full, PS_CHARGE_FULL );	

				/* reading into contents of the files */
				read_file_line( filename_capacity, buff_capacity );
				read_file_line( filename_charge_full_design, buff_charge_full_design );
				read_file_line( filename_charge_full, buff_charge_full );

				int cap = atoi( buff_capacity );
				int q_full = atoi( buff_charge_full );
				int q_design = atoi( buff_charge_full_design );
				float bat_health = 100 * calc_ratio( q_full, q_design );
				float bat_worn_out = 100 - bat_health;

				/* write into notification stats file, notification battery stats for notifications to read and display */
				char *filename_stats = malloc( BUFFSIZE );
				strcpy( filename_stats, VAR_WORK_PATH );
				strcat( filename_stats, "notification_stats" );

				char buf[BUFFSIZE];

				if ( strncmp( buff_capacity, "non", 3 ) != 0 ){

					sprintf( buf, "Charge Capacity\t\t:\t%d %%\n", cap );
					write_file_line_as_char( filename_stats, buf );
					memset( buf, 0, BUFFSIZE*sizeof (char) );

					sprintf( buf, "Battery Health\t\t:\t%.2f %%\n", bat_health );
					append_file_line_as_char( filename_stats, buf );
					memset( buf, 0, BUFFSIZE*sizeof (char) );

					sprintf( buf, "Battery Worn Out\t\t:\t%.2f %%\n", bat_worn_out );
					append_file_line_as_char( filename_stats, buf );
					memset( buf, 0, BUFFSIZE*sizeof (char) );

				}

				free( filename_stats );
			}

			memset( buff_capacity, 0, BUFFSIZE );

			index++;
			sleep( 1 );								// sleep for 1 second/periodic loop time/refresh rate = 1s
			get_power_modes( power_modes );

		}


		free( buff_charge_full_design );
		free( buff_charge_full );
		free( buff_capacity );
		free( filename_charge_full_design );
		free( filename_charge_full );
		free( filename_capacity );

	}


	free( VAR_WORK_PATH );
	free( read_data_filename );
	free( write_data_filename );
	free( data_buffer );


//----------------------------------------------------------------------------------------------------------------------------------------------------------------

	syslog( LOG_NOTICE, "Exiting batmand daemon. Closing down..." );

}

int main(){

	/*
	 * Forks the main calling process and creates the batmand daemon
	 */
	batman_daemon();

	/*
	 * Uses while loop ot monitor battery/ power supply events, collecting data and data analysis
	 * and updating power supply stats. 
	 */
	batman_daemon_detective();

	return EXIT_SUCCESS;
}
