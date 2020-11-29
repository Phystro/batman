#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "batman.h"


char *read_file_line( char *filename, char *read_data_buffer ){

	FILE *fp = fopen( filename, "r" );
	
	if ( fp != NULL ){
		fgets( read_data_buffer, BUFFSIZE, fp );
		fclose( fp );
	} else{
		strcpy(read_data_buffer, "non");
	}

	return read_data_buffer;
}


void write_file_line( char *filename, double data ){

	char buffer[BUFFSIZE];

	FILE *fp = fopen( filename, "w" );
	sprintf(buffer, "%f\n", data);					// convert data from double to string
	fputs(buffer, fp);						// write string data into file
	fclose(fp);
}


void append_file_line( char *filename, double data ){

	char buffer[BUFFSIZE];

	FILE *fp = fopen( filename, "a" );
	sprintf( buffer, "%f\n", data );
	fputs( buffer, fp );
	fclose(fp);
}

