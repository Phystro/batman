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
		"type", "manufacturer", "model_name", "technology", "serial_number",
		"present", "alarm", "online", "capacity_level",
		"charge_full_design", "voltage_min_design", "capacity" 
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

		printf( "\n[+] Power Supply: %s\n", power_modes[i] );

		for ( int j = 0; j < 12; j++ ){
			
			if ( info_files[j] == NULL )
				break;

			strcpy( filename, POWER_SUPPLY_DIR );	
			strcat( filename, power_modes[i] );
			strcat( filename, "/" );
			strcat( filename, info_files[j] );

			read_file_line( filename, read_data_buffer );
			
			if ( strncmp( read_data_buffer, "non", 3 ) != 0 ){
				if ( j == 9 ){
					printf( "\t%-45s: %-8.0f\t[mAh]\n", info_files_headers[j], strtod( read_data_buffer, NULL )/1000 );
				} else if ( j == 10 ){
					printf( "\t%-45s: %-8.2f\t[V]\n", info_files_headers[j], strtod( read_data_buffer, NULL )/1000000 );
				} else if ( j == 11 ){
					printf( "\t%-45s: %-8d\t[%]\n", info_files_headers[j], atoi( read_data_buffer ) );
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
void display_stats(){

	const int count_stats_files = 7;

	const char * stats_files[7] = 
	{
		"charge_full_design", "charge_full", "charge_now", "current_now",
		"voltage_min_design", "voltage_now", "status"
	};

	const char * stats_files_headers[7] = 
	{
		"Full Design Charge Capacity", "Full Usage Charge Capacity", "Present Charge Capacity",
		"Present Current", "Minimum Design Voltage", "Present voltage", "Battery Status"
	};

	const char * stats_derived[4] = 
	{
		"Power Consumption", "% Usage Charge Capacity", "% Design Charge Capacity",
		"% Last Full Charge Capacity"
	};


	double base_stats[6] = {0};

	char *filename = malloc( BUFFSIZE );
	char *read_data_buffer = malloc( BUFFSIZE );
	char *power_modes[BUFFSIZE] = {0}; 

	get_power_modes( power_modes );

	for ( int i = 0; ; i++ ){
		
		if ( power_modes[i] == NULL )
			break;

		printf( "\n[+] Power Supply Stats: %s\n", power_modes[i] );

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
					printf("\t%-30s : %s \n", stats_files_headers[j], read_data_buffer );
				}

			} else{
				continue;
			}

			memset( read_data_buffer, 0, BUFFSIZE );
			memset( filename, 0, BUFFSIZE );
		}

		// full usage charge capacity / full design charge capacity
		printf( "\t%-30s : %g [%]\t [ % BATTERY HEALTH ] \n", stats_derived[2], 100 * calc_ratio( base_stats[1], base_stats[0] ) );

		// Power consumption = Io * Vo
		printf( "\t%-30s : %g [W] \n", stats_derived[0], calc_product( base_stats[3], base_stats[5] ) );

		// Last full charge capacity = present charge capacity / full usage charge capacity
		if ( calc_ratio( base_stats[2], base_stats[1] ) >= 0.9999 ){
			printf( "\t%-30s : %g [%] \t [ FULLY CHARGED ] \n", stats_derived[3], 100 * calc_ratio( base_stats[2], base_stats[1] ) );
		} else if ( ( calc_ratio( base_stats[2], base_stats[1] ) <= 0.21 ) && ( calc_ratio( base_stats[2], base_stats[1] ) > 0.09 ) ){
			printf( "\t%-30s : %g [%] \t [ LOW CHARGE ] \n", stats_derived[3], 100 * calc_ratio( base_stats[2], base_stats[1] ) );
		} else if ( calc_ratio( base_stats[2], base_stats[1] ) <= 0.09 ){
			printf( "\t%-30s : %g [%] \t [ EXTREMELY LOW CHARGE ] \n", stats_derived[3], 100 * calc_ratio( base_stats[2], base_stats[1] ) );
		} else {
			printf( "\t%-30s : %g [%] \t [ NORMAL CHARGE ] \n", stats_derived[3], 100 * calc_ratio( base_stats[2], base_stats[1] ) );
		}

		// % Usage charge capacity = present charge capacity / full design charge capacity
		printf( "\t%-30s : %g [%]\n", stats_derived[1], 100 * calc_ratio( base_stats[2], base_stats[0] ) );

		// memset( base_stats, 0, 6 );
	}

	printf("\n");
	free( filename );
	free( read_data_buffer );
}

