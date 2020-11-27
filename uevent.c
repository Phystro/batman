#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "batman.h"


/*
char *info_files[count_info_files] = 
{
	"type", "manufacturer", "model_name", "technology", "serial_number",
	"present", "alarm", "online", "capacity_level",
	"charge_full_design", "voltage_min_design", "capacity"
}

char *info_files_headers[count_info_files] = 
{
	"Power Supply Type", "Battery Manufacturer", "Battery Model Name", "Battery Technology",
	"Battery Serial Number", "Battery Presence", "Battery Alarm", "Power Supply Online",
	"Battery Capacity Level", "Full Manufacturer Design Charge",
	"Minimum Manufacturer Design Voltage", "Current Battery Charge Capacity"
}
*/

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
			strcat(filename, info_files[j]);

			read_file_line(filename, read_data_buffer);
			
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

	free( filename );
	free( read_data_buffer );
}
