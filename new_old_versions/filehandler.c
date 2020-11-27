#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "batman.h"


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


void create_data_files(char temp_dir[]){

	char temp_path[BUFFSIZE];

	for ( int i = 0; i < nodf; i++ ){
		strcpy(temp_path, temp_dir);
		strcat(temp_path, "/");
		strcat(temp_path, dataFiles[i]);
		printf("temp_path, dataFiles[%d]: %s\n", i, temp_path);

		FILE *fp = fopen(temp_path, "a+");
		fclose(fp);

		// memset(temp_path, 0, BUFFSIZE*sizeof temp_path );
		// temp_path[BUFFSIZE] = {0};
		// bzero(temp_path, sizeof temp_path);
		// temp_path[0] = 0;
	}
}



char *read_file(char *filename){
	char buffer[BUFFSIZE];

	FILE *fp = fopen(filename, "r");
	if ( fp < 0 )
		error("[-] Failed to open file");

	fgets(buffer, BUFFSIZE, fp);
	fclose(fp);

	printf("Read buffer: %s\n", buffer);

	return buffer;
}


void write_file(char *filename, double data){
	FILE *fp = fopen(filename, "w");
	char buffer[BUFFSIZE];
	sprintf(buffer, "%f\n", data);
	fputs(buffer, fp);
	fclose(fp);
}


void append_file(char *filename, double data){
	FILE *fp = fopen(filename, "a");
	char buffer[BUFFSIZE];
	sprintf(buffer, "%f\n", data);
	fputs(buffer, fp);
	fclose(fp);
}


void update_data(char *power_modes[], char batman_work_dir[]){

	/*
	 * Stop recording and collecting data when usage capacity reaches 100.0% to prevent
	 * 	collecting bad irrelevant data.
	 * SI units used in saved data in files. Voltage in V, Current in mA, Charge in mAh
	 */

	char *tmp_refdir = malloc(BUFFSIZE);
	char *tmp_datdir = malloc(BUFFSIZE);
	char *values = malloc(BUFFSIZE);
	double dataTelemetry[10];

	for ( int i = 0; ; i++ ){

		memset(dataTelemetry, 0, 10);

		if ( *(power_modes + i) == NULL ){
			printf("Ran out of power modes\n");
			break;
		}

		// collecting data from each of the files in each power supply
		for (int j = 0; j < 6; j++){
			// memset(tmp_refdir, 0, BUFFSIZE);
			// printf("tmp_refdir clreing\n");
			// memset(values, 0, BUFFSIZE);
			// printf("cleared tmp_refdir and values");

			printf("copying addresses\n");
			strcpy(tmp_refdir, POWER_SUPPLY_DIR);
			strcat(tmp_refdir, *(power_modes + i) );
			strcat(tmp_refdir, "/");
			strcat(tmp_refdir, refFiles[j]);
			printf("finished copying addresses: %s\n", tmp_refdir);
			
			values = read_file(tmp_refdir);
			printf("read into files: values = %s\n", values);
			dataTelemetry[j] = strtod(values, NULL) / 1000;	// converts strings/char * to double
			printf("values: %d = %s | dataTelemetry[%d] = %f\n", j, values, j, dataTelemetry[j]/1000);

			// memset(tmp_refdir, 0, BUFFSIZE);
			// memset(values, 0, BUFFSIZE);
		}
	}
}