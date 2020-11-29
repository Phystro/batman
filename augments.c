#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>			// reading from user database
#include <dirent.h>			// getting directory names and structures
#include <signal.h>
#include <libnotify/notify.h>

#include "batman.h"


void error(char *report){
	perror(report);
	exit(1);
}


double calc_product( double num1, double num2 ){
	
	double product = num1 * num2;

	return product;
}

double calc_ratio( double num, double den ){

	double ratio = num / den;

	return ratio;
}


int get_proc_id_by_name( char *proc_name ){

	/*
	 * Returns PID if success; -1 on Error or failed objecttive
	 */
	pid_t pid = -1;

	char *buffer = malloc( BUFFSIZE );
	char *cmd_path = malloc( BUFFSIZE );

	// Open /proc directory
	DIR *dp = opendir( "/proc" );

	if ( dp != NULL ){
	
		// enumerate all entries in the directory until the process is found
		struct dirent *dirp;

		while ( pid < 0 && ( dirp = readdir( dp ) ) ){
		
			// skip non numeric entries
			pid_t id = atoi( dirp->d_name );
			if ( id > 0 ){
			
				// read the contents of the virtual /proc/{pid}/cmdline file
				strcpy( cmd_path, "/proc/" );
				strcat( cmd_path, dirp->d_name );
				strcat( cmd_path, "/" );
				strcat( cmd_path, "cmdline" );

				// open the file and read its contents
				FILE *fp = fopen( cmd_path, "r" );
				if ( fp != NULL ){
				
					fgets( buffer, BUFFSIZE, fp );
					fclose( fp );
				}

				/*
				 * Needs a string handling function for strings that aren't that straight forward
				 * 	strings with '/' and '.' or './' in them
				 * A more generic string handling function
				 */
				if ( strcmp( buffer, proc_name ) == 0 ){
					pid = id;
				}
			}
		}
	}

	closedir( dp );
	free( buffer );
	free( cmd_path );

	// second confirmation stage using kill command while passing a 0 signal to it
	if ( pid > 0 ){						// process that exists
		int signal = kill( pid, 0 );
		if ( signal == 0 )
			return pid;				// process exists
		else if ( signal < 0 )
			return pid;				// process doesn't exist
	} else
		return pid;					// process doesn't exist
}


int get_ppid_by_pid( const pid_t pid ){

	char buffer[BUFFSIZE];
	pid_t ppid;

	sprintf( buffer, "/proc/%d/stat", pid );

	FILE *fp = fopen( buffer, "r" );

	if ( fp ){
	
		size_t size = fread( buffer, sizeof (char), sizeof (buffer), fp );

		if ( size > 0 ){
			
			strtok( buffer, " " );		// (1) pid %d
			strtok( NULL, " " );		// (2) comm %s
			strtok( NULL, " " );		// (3) state %c

			char *s_ppid = strtok( NULL, " " );		// (4) ppid %d
			ppid = atoi( s_ppid );
		}
	}

	fclose( fp );

	return ppid;
}


/*
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
*/

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


void display_notifications( char *report ){

	char *title = "Battery Notification: STATUS HERE";
	char *msg = "Current Charge\nConnect your laptop for Charging\nDISCONNECT LAPTOP\nFULLY CHARGED";

	GError *error = NULL;
	char name[40] = "Battery Notifications";

	// initialize notify
	notify_init( name );

	// create a new notification
	NotifyNotification *full_charge;
	full_charge = notify_notification_new( title, msg, "/root/Pictures/gnome_battery_caution.ico" );

	//set timeout
	notify_notification_set_timeout( full_charge, 10000 );	// 10 secs

	// set app name
	notify_notification_set_app_name( full_charge, "BATMAN" );

	// set urgency
	notify_notification_set_urgency( full_charge, NOTIFY_URGENCY_CRITICAL );

	// add action
	// 

	// show notification
	notify_notification_show( full_charge, &error );
}


void display_usage(){

	printf( "__\n|Batman v0.0.1\nEnergy consumption, power usage and battery monitoring tool.\n\nUSAGE:\n\tbatman [OPTIONS]...\n\nOPTIONS:\n\t-i, --info\t\tDisplay summarised battery and/or AC Mains information \n\t-s, --stats\t\tDisaply battery or AC Mains stats information \n\t    --start_daemon\tStart batman daemon if the daemon was stopped \n\t    --stop_daemon\tStop batman daemon if the daemon is running  \n" );
	
	exit( EXIT_FAILURE );
}

