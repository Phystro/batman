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


char *red(){ return ("\033[0;31m"); }
char *redb(){ return ("\033[1;31m"); }
char *green(){ return ("\033[0;32m"); }
char *greenb(){ return "\033[1;32m"; }
char *yellow(){ return ("\033[0;33m"); }
char *yellowb(){ return ("\033[1;33m"); }
char *blue(){ return ("\033[0;34m"); }
char *blueb(){ return ("\033[1;34m"); }
char *magenta(){ return ("\033[0;35m"); }
char *magentab(){ return ("\033[1;35m"); }
char *cyan(){ return ("\033[0;36m"); }
char *cyanb(){ return ("\033[1;36m"); }
char *resetc(){ return ("\033[0m"); }


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

	// char *buffer = malloc( BUFFSIZE );
	char buffer[BUFFSIZE];
	char *proc_enc_name = malloc( BUFFSIZE );
	char *stat_path = malloc( BUFFSIZE );

	//enclose proc name in brackets just like in the proc files
	strcpy( proc_enc_name, "(" );
	strcat( proc_enc_name, proc_name );
	strcat( proc_enc_name, ")" );

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
				strcpy( stat_path, "/proc/" );
				strcat( stat_path, dirp->d_name );
				strcat( stat_path, "/" );
				strcat( stat_path, "stat" );

				// open the file and read its contents
				FILE *fp = fopen( stat_path, "r" );
				if ( fp != NULL ){
				
					size_t  size = fread( buffer, sizeof (char), sizeof (buffer), fp );

					if ( size > 0 ){
			
						strtok( buffer, " " );							// (1) pid %d
						char *proc_stat_name = strtok( NULL, " " );		// (2) comm %s

						if ( strcmp( proc_enc_name, proc_stat_name ) == 0 ){
							pid = id;
						}
					}
				}
				fclose( fp );
			}
		}
	}

	closedir( dp );
	free( proc_enc_name );
	free( stat_path );

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

int pid_has_tty( int pid ){
	/*
	 * query the controlling tty of a process
	 * /proc/{pid}/fd/0 is a symbolink link to the tty say (/dev/pts/4)
	 * use the readlink system call to get the tty.
	 */
	pid_t proc_id = pid;

	char symlink_path[BUFFSIZE];
	char buffer[BUFFSIZE];

	sprintf( symlink_path, "/proc/%d/fd/0", proc_id );
	int ret = readlink( symlink_path, buffer, BUFFSIZE );
	buffer[ret] = '\0';

	if ( strncmp( buffer, "/dev/pts/", 9 ) == 0 ){
		// process has a tty
		return 1;				// yes tty
	} else{
		// has no tty. Buffer contains the string socket:[......]
		return 0;				// no tty
	}
}



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


void display_notifications( char *title_name, char *title_report, int URGENCY, char *caution_report, const char *icon_pathname ){

	/*
	 * Priorities
	 * 	0 = low
	 * 	1 = normal
	 * 	2 = critical
	 */

	char *title = malloc( BUFFSIZE );
	strcpy( title, "Battery Notification:\t" );
	strcat( title, title_name );
	strcat( title, "\n" );
	// strcat( title, "Status:\t");
	
	if ( title_report != NULL )
		strcat( title, title_report );

	char *message = malloc( BUFFSIZE );
	strcpy( message, "Last full Charge Capacity; Remaining Time\nBattery Health\nBattery Worn Out\n WARNING MESSAGE\n" );
	
	if ( caution_report != NULL )
		strcat( message, caution_report );

	const char *icon_path = icon_pathname;

	GError *error = NULL;
	char name[40] = "Battery Notifications";

	// initialize notify
	notify_init( name );

	// create a new notification
	NotifyNotification *bat_notify;
	bat_notify = notify_notification_new( title, message, icon_path );	

	//set timeout
	// notify_notification_set_timeout( bat_notify, INTERVAL * 200 );	// 12 secs

	// set app name
	notify_notification_set_app_name( bat_notify, "batman_notify" );

	// set urgency
	notify_notification_set_urgency( bat_notify, URGENCY );

	// set hint
	notify_notification_set_hint( bat_notify, "resident", g_variant_new_boolean( TRUE ) );	// transient	// resident

	// show notification
	notify_notification_show( bat_notify, &error );
		
	// add action
	// 

	// free( icon_path );
	free( title );
	free( message );

	//notify_uninit();
}


void display_usage(){

	printf( "__\n|Batman v0.0.1\nEnergy consumption, power usage and battery monitoring tool.\n\nUSAGE:\n\tbatman [OPTIONS]...\n\nOPTIONS:\n\t-i, --info\t\tDisplay summarised battery and/or AC Mains information \n\t-s, --stats\t\tDisaply battery or AC Mains stats information \n\t    --start_daemon\tStart batman daemon if the daemon was stopped \n\t    --stop_daemon\tStop batman daemon if the daemon is running  \n" );
	
	exit( EXIT_FAILURE );
}

