#include "../include/batman.h"


void errorlog(char *report){
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


int get_ppid_by_pid( const int pid ){

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


int verify_cmdline( int pid, char *cmdline, int char_length ){

	int verification = 0;
	char *buf_filename = malloc( BUFFSIZE );
	sprintf( buf_filename, "/proc/%d/cmdline", pid );

	char *buf_cmdline = malloc( BUFFSIZE );

	FILE *fp = fopen( buf_filename, "r" );

	if ( fp ){
	
		fgets( buf_cmdline, BUFFSIZE, fp );
		if ( strncmp( buf_cmdline, cmdline, char_length ) == 0 )
			verification = 1;					// true
		else
			verification = 0;					// false
	}

	fclose( fp );

	free( buf_cmdline );
	free( buf_filename );

	return verification;
}


char *get_home_dir(){

	// getting $HOME directory from environment variable HOME
	char *home_dir = getenv("HOME");

	if (home_dir != NULL){
		// printf("HOME dir from environment: %s\n", home_dir);
		return home_dir;
	}

	// get $HOME directory from system user database
	uid_t uid = getuid();
	struct passwd *pw = getpwuid(uid);

	if (pw == NULL){
		// failed to get $HOME
		exit(EXIT_FAILURE);
	}

	// printf("HOME from user db: %s\n", pw->pw_dir);
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


void play_sound_effects( const char *sound_effect ){

//	char command[BUFFSIZE];
//	sprintf( command, "aplay -c 1 -q -t wav %s", sound_effect );
//	system( command );


	mpg123_handle *mh;
	unsigned char *buffer;
	size_t buffer_size;
	size_t done;
	int err;

	int driver;
	ao_device *device;

	ao_sample_format format;
	int channels, encoding;
	long rate;

	/* Initializations */
	ao_initialize();
	driver = ao_default_driver_id();
	mpg123_init();

	mh = mpg123_new( NULL, &err );
	buffer_size = mpg123_outblock( mh );
	buffer = ( unsigned char * ) malloc( buffer_size * sizeof( unsigned char ) );

	/* open the file and get the decoding format */
	mpg123_open( mh, sound_effect );
	mpg123_getformat( mh, &rate, &channels, &encoding );

	/* set the output format and open the output device */
	format.bits = mpg123_encsize( encoding ) * BITS;
	format.rate = rate;
	format.channels = channels;
	format.byte_format = AO_FMT_NATIVE;
	format.matrix = 0;
	device = ao_open_live( driver, &format, NULL );

	/* decode and play */
	while ( mpg123_read( mh, buffer, buffer_size, &done) == MPG123_OK )
		ao_play( device, buffer, done );

	/* clean up */
	free( buffer );
	ao_close( device );
	mpg123_close( mh );
	mpg123_delete( mh );
	mpg123_exit();
	ao_shutdown();
}


void display_notifications( NotifyNotification *notify_batman, GError *error, char *PS_NAME, char *header, u_int8_t URGENCY, char *caution_report, const char *icon_pathname ){
	/*
	 * Priorities
	 * 	0 = low
	 * 	1 = normal
	 * 	2 = critical
	 */

	char *title = malloc( BUFFSIZE );
	strcpy( title, header );
	strcat( title, "\t:\t" );

	char *cap_filename = malloc( BUFFSIZE );
	char *cap_str_value = malloc( BUFFSIZE );
	char *cap_value = malloc( BUFFSIZE );
	strcpy( cap_filename, POWER_SUPPLY_DIR );
	strcat( cap_filename, PS_NAME );
	strcat( cap_filename, "/" );
	strcat( cap_filename, PS_CAPACITY );
	read_file_line( cap_filename, cap_str_value );

	/* remove endline character from cap_str_value */
	for ( uint8_t i = 0;  ; i++){
		if ( cap_str_value[i] == '\n' ){
			cap_value[i] = '\0';
			break;
		}
		cap_value[i] = cap_str_value[i];
	}
	
	strcat( title, cap_value );
	strcat( title, " %\t:\t" );
	strcat( title, PS_NAME );
	strcat( title, "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\n" );

	// get stats file path/location
	char *stats_filename = malloc( BUFFSIZE );
	char *VAR_WORK_PATH = malloc( BUFFSIZE );
	strcpy( VAR_WORK_PATH, "/var/lib/" );
	strcat( VAR_WORK_PATH, VAR_WORK_DIR );
	strcpy( stats_filename, VAR_WORK_PATH );
	strcat( stats_filename, "notification_stats" );

	char *message = malloc( BUFFSIZE );
	read_file_content( stats_filename, message );
	/*
	 * TODO: remember to add info on remaining time
	*/
	
	if ( caution_report != NULL ){
		strcat( message, "\n" );
		strcat( message, caution_report );
	}

	/* set app name */
	notify_notification_set_app_name( notify_batman, "batman_notify" );

	/* set urgency */
	notify_notification_set_urgency( notify_batman, URGENCY );

	// set hint
	// notify_notification_set_hint( notify_batman, "transient", g_variant_new_boolean( FALSE ) );	// transient	// resident

	/* update notification */
	notify_notification_update( notify_batman, title, message, icon_pathname );

	if ( error != NULL )
		exit( EXIT_FAILURE );

	/* show notification */
	notify_notification_show( notify_batman, &error );
		
	free( cap_filename );
	free( cap_str_value );
	free( title );
	free( message );
	free( VAR_WORK_PATH );
	free( stats_filename );
}
