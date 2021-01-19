#include "../linux-headers/include/batman.h"

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


char *read_file_content( char *filename, char *read_data_content ){

	int index = 0;
	int n;
	char *content = malloc( BUFFSIZE );

	FILE *fp = fopen( filename, "r" );

	if ( fp != NULL ){

		while ( (n = fgetc( fp ) ) != EOF ){

			*( content + index ) = n;
			index++;
		}

		strcpy( read_data_content, content );
	
	} else{
		strcpy( read_data_content, "non" );
	}

	free( content );

	return read_data_content;
}


void write_file_line( char *filename, double data ){

	char buffer[BUFFSIZE];

	FILE *fp = fopen( filename, "w" );
	sprintf( buffer, "%f\n", data );			// convert data from double to string
	fputs( buffer, fp );					// write string data into file
	fclose( fp );
}


void write_file_line_as_char( char *filename, char *data ){

	FILE *fp = fopen( filename, "w" );
	fputs( data, fp );						// write string data into file
	fclose( fp );
}


void append_file_line( char *filename, double data ){

	char buffer[BUFFSIZE];

	FILE *fp = fopen( filename, "a" );
	sprintf( buffer, "%f\n", data );
	fputs( buffer, fp );
	fclose( fp );
}

void append_file_line_as_char( char *filename, char *data ){

	FILE *fp = fopen( filename, "a" );
	fputs( data, fp );
	fclose( fp );
}

