#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "batman.h"


/* 
 * Info Function call to display quick bat/AC Mains info
 */
void display_info(){
	
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


/*
 * Information function call to display all battery stats i.e. usage, performance, percentage, 
 * 	consumption, wearing, rates....
 */
void display_stats( ){

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

