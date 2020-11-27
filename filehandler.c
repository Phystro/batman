#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "batman.h"

/*
int nodf = 10;
char *dataFiles[10] = {
	"present_voltage", "present_current", "present_charge", "full_usage_charge",
	"min_design_voltage", "full_design_charge",
	"power_consumption","usage_charge_capacity", "design_charge_capacity", "supply_voltage_ratio"
};


int norf = 6; 
char *refFiles[6] = {
	"voltage_now", "current_now", "charge_now", "charge_full",
	"voltage_min_design", "charge_full_design"
};
*/

char *read_file_line( char *filename, char *read_data_buffer ){

	FILE *fp = fopen( filename, "r" );
	
	if ( fp != NULL ){
		fgets( read_data_buffer, BUFFSIZE, fp );
		fclose( fp );
	} else{
		strcpy(read_data_buffer, "non");
	}
}


void write_file( char *filename, double data ){

	char buffer[BUFFSIZE];

	FILE *fp = fopen( filename, "w" );
	sprintf(buffer, "%f\n", data);					// convert data from double to string
	fputs(buffer, fp);						// write string data into file
	fclose(fp);
}


void append_file( char *filename, double data ){

	char buffer[BUFFSIZE];

	FILE *fp = fopen( filename, "a" );
	sprintf( buffer, "%f\n", data );
	fputs( buffer, fp );
	fclose(fp);
}

