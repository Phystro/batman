#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include "batman.h"


void display_info(){

	/* 
 	 * Function to display quick battery/AC Mains information
 	 */	
	const int count_info_files = 12;

	const char *info_files[12] = 
	{ 
		PS_TYPE, PS_MANUFACTURER, PS_MODEL_NAME, PS_TECHNOLOGY, PS_SERIAL_NUMBER,
		PS_PRESENT, PS_ALARM, PS_ONLINE, PS_CAPACITY_LEVEL,
		PS_CHARGE_FULL_DESIGN, PS_VOLTAGE_MIN_DESIGN, PS_CAPACITY 
	};

	const char *info_files_headers[12] = 
	{
		"Power Supply Type", "Battery Manufacturer", "Battery Model Name", "Battery Technology",
		"Battery Serial Number", "Battery Presence", "Battery Alarm Value", "Power Supply Online",
		"Battery Capacity Level", "Full Manufacturer Design Charge",
		"Minimum Manufacturer Design Voltage", "Current Battery Charge Capacity"
	};

	char *filename = malloc( BUFFSIZE );
	char *read_data_buffer = malloc( BUFFSIZE );
	char *power_modes[BUFFSIZE] = {0}; 

	get_power_modes( power_modes );
	
	for ( int i = 0; ; i++ ){
	
		if ( power_modes[i] == NULL )
			break;

		printf( "\n%s[+]%s Power Supply: %s%s%s\n", greenb(), resetc(), magentab(), power_modes[i], resetc() );

		for ( int j = 0; j < 12; j++ ){
			
			if ( info_files[j] == NULL )
				break;

			strcpy( filename, POWER_SUPPLY_DIR );	
			strcat( filename, power_modes[i] );
			strcat( filename, "/" );
			strcat( filename, info_files[j] );

			read_file_line( filename, read_data_buffer );
			
			if ( strncmp( read_data_buffer, "non", 3 ) != 0 ){
				if ( j == 5 ){
					if ( strncmp( read_data_buffer, "1", 1 ) == 0 )
						printf( "\t%-45s: %s%-8s%s\n", info_files_headers[j], greenb(), "yes", resetc() );			// battery present
					else
						printf( "\t%-45s: %s%-8s%s\n", info_files_headers[j], redb(), "no", resetc() );			// battery absent
				} else if ( j == 9 ){
					printf( "\t%-45s: %-8.0f\t[mAh]\n", info_files_headers[j], strtod( read_data_buffer, NULL )/1000 );
				} else if ( j == 7 ){
					if ( strncmp( read_data_buffer, "1", 1 ) == 0 )	
						printf( "\t%-45s: %s%-8s%s\n", info_files_headers[j], greenb(), "ON", resetc() );			// battery/mains present
					else
						printf( "\t%-45s: %s%-8s%s\n", info_files_headers[j], redb(), "OFF", resetc() );			// battery/mains  absent
				}else if ( j == 10 ){
					printf( "\t%-45s: %-8.2f\t[V]\n", info_files_headers[j], strtod( read_data_buffer, NULL )/1000000 );
				} else if ( j == 11 ){
					// battery charge capacity
					int bat_capacity = atoi( read_data_buffer );
					if ( (bat_capacity == 100) ||  (bat_capacity < 11) )
						printf( "\t%-45s: %s%-8d\t[%%]%s\n", info_files_headers[j], redb(), bat_capacity, resetc() );
					else if( bat_capacity >= 11 && bat_capacity <= 21)
						printf( "\t%-45s: %s%-8d\t[%%]%s\n", info_files_headers[j], yellowb(), bat_capacity, resetc() );
					else
						printf( "\t%-45s: %s%-8d\t[%%]%s\n", info_files_headers[j], greenb(), bat_capacity, resetc() );
				} else{
					printf( "\t%-45s: %-8s", info_files_headers[j], read_data_buffer );
				}
			} else{
				continue;
			}

			memset( read_data_buffer, 0, BUFFSIZE );
			memset( filename, 0, BUFFSIZE );
		}
	}

	printf("\n");
	free( filename );
	free( read_data_buffer );
}


void display_stats( ){

	/*
 	 * Information function call to display all battery stats i.e. usage, performance, percentage, 
 	 * 	consumption, wearing, rates....
 	 */

	const int count_stats_files = 7;

	const char * stats_files[7] = 
	{
		PS_CHARGE_FULL_DESIGN, PS_CHARGE_FULL, PS_CHARGE_NOW, PS_CURRENT_NOW,
		PS_VOLTAGE_MIN_DESIGN, PS_VOLTAGE_NOW, PS_STATUS
	};

	const char * stats_files_headers[7] = 
	{
		"Full Design Charge Capacity", "Full Usage Charge Capacity", "Present Charge Capacity",
		"Present Current", "Minimum Design Voltage", "Present voltage", "Battery Status"
	};

	const char * stats_derived[6] = 
	{
		"Full Design Energy Usage", "Current Energy Usage", "Power Consumption", "% Usage Charge Capacity", "% Design Charge Capacity",
		"% Last Full Charge Capacity"
	};


	double base_stats[6] = {0};

	char *filename = malloc( BUFFSIZE );
	char *read_data_buffer = malloc( BUFFSIZE );
	char *power_modes[BUFFSIZE] = {0}; 

	get_power_modes( power_modes );

	for ( int i = 0; ; i++ ){

		memset( base_stats, 0, sizeof (base_stats) );
		
		if ( power_modes[i] == NULL )
			break;

		printf( "\n%s[+]%s Power Supply Stats: %s%s%s\n", greenb(), resetc(), magentab(), power_modes[i], resetc() );

		for ( int j = 0; j < 7; j++ ){

			if ( stats_files[j] == NULL )
				break;

			strcpy( filename, POWER_SUPPLY_DIR );
			strcat( filename, power_modes[i] );
			strcat( filename, "/" );
			strcat( filename, stats_files[j] );

			read_file_line( filename, read_data_buffer );

			if ( strncmp( read_data_buffer, "non", 3 ) != 0 ){
				
				if ( (j == 0) || (j == 1) || (j == 2) ){
					printf("\t%-30s : %g\t[mAh]\n", stats_files_headers[j], strtod( read_data_buffer, NULL )/1000 );
					base_stats[j] = strtod( read_data_buffer, NULL ) / 1000000;
				} else if ( j == 3 ){
					printf("\t%-30s : %g\t[mA]\n", stats_files_headers[j], strtod( read_data_buffer, NULL )/1000 );
					base_stats[j] = strtod( read_data_buffer, NULL ) / 1000000;
				} else if ( (j==4) || (j==5) ){
					printf("\t%-30s : %g\t[V]\n", stats_files_headers[j], strtod( read_data_buffer, NULL )/1000000 );
					base_stats[j] = strtod( read_data_buffer, NULL ) / 1000000;
				} else {
					// battery status
					printf("\t%-30s : %s \n", stats_files_headers[j], read_data_buffer );
				}

			} else{
				continue;
			}

			memset( read_data_buffer, 0, BUFFSIZE );
			memset( filename, 0, BUFFSIZE );
		}

		if ( strlen( (char *) (base_stats)) != 0 ){

			// Last full charge capacity = present charge capacity / full usage charge capacity
			if ( calc_ratio( base_stats[2], base_stats[1] ) >= 0.9999 ){
				printf( "\t%-30s : \033[1;31m%g [%%] \t [ FULLY CHARGED ] \033[0m \n", stats_derived[5], 100 * calc_ratio( base_stats[2], base_stats[1] ) );
			} else if ( ( calc_ratio( base_stats[2], base_stats[1] ) <= 0.21 ) && ( calc_ratio( base_stats[2], base_stats[1] ) > 0.09 ) ){
				printf( "\t%-30s : \033[1;33m%g [%%] \t [ LOW CHARGE ] \033[0m \n", stats_derived[5], 100 * calc_ratio( base_stats[2], base_stats[1] ) );
			} else if ( calc_ratio( base_stats[2], base_stats[1] ) <= 0.09 ){
				printf( "\t%-30s : \033[1;31m%g [%%] \t [ EXTREMELY LOW CHARGE ] \033[0m \n", stats_derived[5], 100 * calc_ratio( base_stats[2], base_stats[1] ) );
			} else {
				printf( "\t%-30s : %s%g [%%] \t [ NORMAL CHARGE ]%s \n", stats_derived[5], greenb(), 100 * calc_ratio( base_stats[2], base_stats[1] ), resetc() );
			}


			// % Usage charge capacity = present charge capacity / full design charge capacity
			printf( "\t%-30s : %g [%%]\n", stats_derived[3], 100 * calc_ratio( base_stats[2], base_stats[0] ) );


			// full usage charge capacity / full design charge capacity. Uses a quatile range grading color system; red-yellow-green-blue
			double bat_health = 100 * calc_ratio( base_stats[1], base_stats[0] );
			if ( bat_health >= 75.0 )
				printf( "\t%-30s : %s%g [%%]\t [ %% BATTERY HEALTH ] %s\n", stats_derived[4], blueb(), bat_health, resetc() );
			else if ( bat_health <= 25.0 )
				printf( "\t%-30s : %s%g [%%]\t [ %% BATTERY HEALTH ] %s\n", stats_derived[4], redb(), bat_health, resetc() );
			else if ( bat_health > 25.0 && bat_health <=50.0 )
				printf( "\t%-30s : %s%g [%%]\t [ %% BATTERY HEALTH ] %s\n", stats_derived[4], yellowb(), bat_health, resetc() );
			else if ( bat_health > 50.0 && bat_health < 75.0)
				printf( "\t%-30s : %s%g [%%]\t [ %% BATTERY HEALTH ] %s\n", stats_derived[4], greenb(), bat_health, resetc() );

			// full design energy usage = full design charge * minimum voltage
			printf( "\t%-30s : %g [Wh] \n", stats_derived[0], calc_product( base_stats[0], base_stats[4] ) );
			// full energy usage = full usage charge * miminim voltage
			// printf( "\t%-30s : %g [Wh] \n", stats_derived[1], calc_product( base_stats[1], base_stats[4] ) );
			// energy usage = charge now * voltage now
			printf( "\t%-30s : %g [Wh] \n", stats_derived[1], calc_product( base_stats[2], base_stats[5] ) );

			
			// sprintf( bat_stats[1], "%f", ( 100 * calc_ratio( base_stats[1], base_stats[0] ) ) );			// battery health
			// sprintf( bat_stats[2], "%f", ( 100 * ( 1 - calc_ratio( base_stats[1], base_stats[0] ) ) ) );		// battery worn out

			// Power consumption = Io * Vo
			printf( "\t%-30s : %g [W] \n", stats_derived[2], calc_product( base_stats[3], base_stats[5] ) );

			
			// bat_stats[3] = ( 100 * calc_ratio( base_stats[2], base_stats[1] ) );			// last full charge capacity
			// sprintf( bat_stats[3], "%f", ( 100 * calc_ratio( base_stats[2], base_stats[1] ) ) );

		}

	}

	printf("\n");
	free( filename );
	free( read_data_buffer );
}


void display_usage(){

	printf( "\nBatman v0.0.1\nEnergy consumption, power usage and battery monitoring tool.\n\nUSAGE:\n\tbatman [OPTIONS]...\n\nOPTIONS:\n\t-i, --info\t\tDisplay summarised battery and/or AC Mains information \n\t-s, --stats\t\tDisaply battery or AC Mains stats information \n" );
	
	exit( EXIT_FAILURE );
}


void monitor_events(){

	char *VAR_WORK_PATH = malloc( BUFFSIZE );
	strcpy( VAR_WORK_PATH, "/var/lib/" );
	strcat( VAR_WORK_PATH, VAR_WORK_DIR );

	uint8_t toggle_cap = 0;
	char *toggle_status = malloc( BUFFSIZE );					// for monitoring battery status > charging/discharging states
	strcpy( toggle_status, "Charging" );

	char *power_modes[BUFFSIZE];

	get_power_modes( power_modes );

	uint8_t index = 0;									// counter

	char *filename_charge_full_design = malloc( BUFFSIZE );
	char *filename_charge_full = malloc( BUFFSIZE );
	char *filename_capacity = malloc( BUFFSIZE );
	char *filename_status = malloc( BUFFSIZE );

	char *buff_charge_full_design = malloc( BUFFSIZE );
	char *buff_charge_full = malloc( BUFFSIZE );
	char *buff_status = malloc(BUFFSIZE);
	char *buff_capacity = malloc(BUFFSIZE);

	while ( 1 ){

		for ( uint8_t i = 0; ; i++ ){

				if ( power_modes[i] == NULL )
					break;
				
				/* reading into status filename */
				strcpy( filename_status, POWER_SUPPLY_DIR );
				strcat( filename_status, power_modes[i] );
				strcat( filename_status, "/" );
				strcat( filename_status, PS_STATUS );

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
				read_file_line( filename_status, buff_status );
				read_file_line( filename_capacity, buff_capacity );
				read_file_line( filename_charge_full_design, buff_charge_full_design );
				read_file_line( filename_charge_full, buff_charge_full );

				int cap = atoi( buff_capacity );
				int q_full = atoi( buff_charge_full );
				int q_design = atoi( buff_charge_full_design );
				float bat_health = 100 * calc_ratio( q_full, q_design );
				float bat_worn_out = 100 - bat_health;


				/* spawn notifications based on battery/power supply events */

				if( strncmp( buff_status, "non", 3 ) != 0  && strncmp( buff_capacity, "non", 3 ) != 0 ){

					/* if the battery status changes, spawn notification about the change */
					if ( strcmp( buff_status, toggle_status ) != 0 ){

						strcpy( toggle_status, buff_status );
						get_power_modes( power_modes );
						/*
						 * Adjust the icon images and messages to be displayed on notification
						 * 	depending on reason i.e. state situation
						 * 	e.g. charging, unknown, full...e.t.c.
						 */
						
						if ( strncmp( toggle_status, "Charging", 8 ) == 0 ){
							/* different icons for different charging levels */
							if ( cap <= 10 && cap > 0 )
							{
								const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-charge-empty-battery.png";
								display_notifications( power_modes[i], NULL, toggle_status, 1, NULL, icon );	// set urgency level to normal = 1

							} 
							else if ( cap <= 20 && cap > 10 )
							{
								const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-charging-empty-battery.png";
								display_notifications( power_modes[i], NULL, toggle_status, 1, NULL, icon );	// set urgency level to normal = 1

							}
							else if ( cap < 50 && cap > 20 )
							{
								const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-charging-low-battery.png";
								display_notifications( power_modes[i], NULL, toggle_status, 1, NULL, icon );	// set urgency level to normal = 1

							}
							else if ( cap <= 75 && cap >= 50 )
							{
								const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-medium-charging-battery.png";
								display_notifications( power_modes[i], NULL, toggle_status, 1, NULL, icon );	// set urgency level to normal = 1

							}
							else if ( cap < 100 && cap > 75 )
							{
								const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-battery.png";
								display_notifications( power_modes[i], NULL, toggle_status, 1, NULL, icon );	// set urgency level to normal = 1

							}		
						}


						if ( strncmp( toggle_status, "Discharging", 11 ) == 0 ){
							/* different icons for different discharging levels */
							if ( cap <= 9 )
							{
								char *caution = "\nCRITICALLY LOW CHARGE";
								char *header = "CRITICALLY LOW\t:\t";
								const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-warning-battery.png";
								display_notifications( power_modes[i], header, toggle_status, 1, caution, icon );	// set urgency level to normal = 1

							}
							else if ( cap > 9 && cap <= 19 )
							{
								char *caution = "\nLOW CHARGE";
								char *header = "LOW CHARGE\t:\t";
								const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-nearly-empty-battery.png";
								display_notifications( power_modes[i], header, toggle_status, 1, caution, icon );	// set urgency level to normal = 1

							}
							else if ( cap > 19 && cap < 50 )
							{
								const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-low-battery.png";
								display_notifications( power_modes[i], NULL, toggle_status, 1, NULL, icon );	// set urgency level to normal = 1

							}
							else if ( cap >= 50 && cap < 75 )
							{
								const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-battery-medium-level.png";
								display_notifications( power_modes[i], NULL, toggle_status, 1, NULL, icon );	// set urgency level to normal = 1

							}
							else if ( cap >= 75 && cap < 100 )
							{
								const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-charged-discharging-battery.png";
								display_notifications( power_modes[i], NULL, toggle_status, 1, NULL, icon );	// set urgency level to normal = 1

							}
							else if ( cap == 100 )
							{
								char *caution = "\nFULLY CHARGED";
								char *header = "FULLY CHARGED\t:\t";
								const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-full-battery.png";
								display_notifications( power_modes[i], header, toggle_status, 1, caution, icon );	// set urgency level to normal = 1

							}
						}


						if ( strncmp( toggle_status, "Unknown", 7 ) == 0 ){
							const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-battery-unknown.png";
							char *caution = "\nUNDEFINED BATTERY STATE";
							char *header = "UNDEFINED STATE\t:\t";
							display_notifications( power_modes[i], header, toggle_status, 0, caution, icon );	// set urgency level to normal = 0
						}


						if ( strncmp( toggle_status, "Full", 4 ) == 0 ){
							const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-full-battery.png";
							char *caution = "\nFULLY CHARGED. UNPLUG FROM A/C MAINS";
							char *header = "FULLY CHARGED\t:\t";
							display_notifications( power_modes[i], header, toggle_status, 2, caution, icon );	// urgency level critical = 2
						}
					}


					/* if battery charge capacity goes below certain critically low values, spawn notifications about the low values */
					if ( ( strcmp( buff_status, toggle_status ) == 0 ) && ( strncmp( buff_status, "Discharging", 11 ) == 0 ) ){

							if ( cap == 19 ){
								if ( toggle_cap <= 2 )
									toggle_cap = 1;
							}
							else if ( cap == 9 ){
								if ( toggle_cap <= 2 )
									toggle_cap = 2;
							}
							else
								toggle_cap = 0;


							if ( toggle_cap == 2 ){
								const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-warning-battery.png";
								char *caution = "\nCRITICALLY LOW CHARGE";
								char *header = "CRITICALLY LOW\t:\t";
								display_notifications( power_modes[i], header, toggle_status, 2, caution, icon );	// set urgency level to normal = 2

							}

							if ( toggle_cap == 1 ){
								const char icon[BUFFSIZE] = "/usr/share/pixmaps/batman/icons/icons8/icons8-nearly-empty-battery.png";
								char *caution = "\nLOW CHARGE";
								char *header = "LOW CHARGE\t:\t";
								display_notifications( power_modes[i], header, toggle_status, 1, caution, icon );	// set urgency level to normal = 1
							}

							toggle_cap += 2;
					}
					

				}
				else{

					continue;
				}

		}

			memset( buff_status, 0, BUFFSIZE );
			memset( buff_capacity, 0, BUFFSIZE );

			index++;
			sleep( 1 );								// sleep for 1 second/periodic loop time/refresh rate = 1s
			get_power_modes( power_modes );
	}

	free( buff_charge_full_design );
	free( buff_charge_full );
	free( buff_capacity );
	free( buff_status );
	free( filename_charge_full_design );
	free( filename_charge_full );
	free( filename_capacity );
	free( filename_status );
}