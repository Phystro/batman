#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "batman.h"

char *dataFiles[10] = {
	"present_voltage", "present_current", "present_charge", "full_usage_charge",
	"min_design_voltage", "full_design_charge",
	"power_consumption","usage_charge_capacity", "design_charge_capacity", "supply_voltage_ratio"
};

char *refFiles[6] = {
	"voltage_now", "current_now", "charge_now", "charge_full",
	"voltage_min_design", "charge_full_design"
};

void createDataFiles(char *dirpath){

	char *tmpdir = malloc(BUFFSIZE);

	for (int i = 0; i < 10; i++){
		strcpy(tmpdir, dirpath);
		strcat(tmpdir, "/");
		strcat(tmpdir, dataFiles[i]);
		FILE *fp;			// file pointer
		fp = fopen(tmpdir, "a+");
		fclose(fp);
	}
	free(tmpdir);
}

void readFile(char *filename, char *values){
	FILE *fp = fopen(filename, "r");
	fgets(values, BUFFSIZE, fp);
	fclose(fp);
}

void writeFile(char *filename, double data){
	FILE *fp = fopen(filename, "w");
	char buf[BUFFSIZE];
	sprintf(buf, "%f\n", data);
	fputs(buf, fp);
	fclose(fp);
}

void appendFile(char *filename, double data){
	FILE *fp = fopen(filename, "a");
	char buf[BUFFSIZE];
	sprintf(buf, "%f\n", data);
	fputs(buf, fp);
	fclose(fp);
}

void updateData(char *power_supplies[], char *workDir){

	/*
	 * Stop recording and collecting data when usage capacity reaches 100.0% to prevent
	 * 	collecting bad irrelevant data.
	 * SI units used in saved data in files. Voltage in V, Current in mA, Charge in mAh
	 */

	char *tmp_refdir = malloc(BUFFSIZE);
	char *tmp_datdir = malloc(BUFFSIZE);
	char *values = malloc(BUFFSIZE);
	double dataTelemetry[10];

	for (int i = 0; ; i++){
		if (power_supplies[i] == NULL)
			break;
		// collecting data from each of the files in each power supply
		for (int j = 0; j < 6; j++){
			memset(tmp_refdir, 0, BUFFSIZE);
			memset(values, 0, BUFFSIZE);

			strcpy(tmp_refdir, POWER_SUPPLY_DIR);
			strcat(tmp_refdir, power_supplies[i]);
			strcat(tmp_refdir, "/");
			strcat(tmp_refdir, refFiles[j]);
			
			readFile(tmp_refdir, values);
			dataTelemetry[j] = strtod(values, NULL) / 1000;	// converts strings/char * to double
			printf("values: %d = %s | dataTelemetry[%d] = %f\n", j, values, j, dataTelemetry[j]/1000);
		}
		free(values);
		free(tmp_refdir);
		
		dataTelemetry[6] = (dataTelemetry[0] * dataTelemetry[1]) / 1000000;	// power consumption
		dataTelemetry[7] = (dataTelemetry[2] / dataTelemetry[3]) * 100.0;	// usage charge capacity
		dataTelemetry[8] = (dataTelemetry[3] / dataTelemetry[5]) * 100.0;	// design charge capacity
		dataTelemetry[9] = dataTelemetry[0] / dataTelemetry[4];			// supply voltage ratio
		
		for (int k = 0; k < 11; k++){
			memset(tmp_datdir, 0, BUFFSIZE);
			
			strcpy(tmp_datdir, workDir);
			strcat(tmp_datdir, power_supplies[i]);
			strcat(tmp_datdir, "/");
			strcat(tmp_datdir, dataFiles[k]);

			// change present voltage from kV to V
			if ( k == 0 ){
				appendFile(tmp_datdir, dataTelemetry[k] / 1000);		// present voltage
			}
			// for the constant files/data
			else if ( k == 4 ){
				writeFile(tmp_datdir, dataTelemetry[k] / 1000);	// voltage min design, normal
			} else if ( k == 5){
				writeFile(tmp_datdir, dataTelemetry[k]);		// charge full design
			}else{
				appendFile(tmp_datdir, dataTelemetry[k]);
			}
			printf("decTelemetry[%d] = %f\n", k, dataTelemetry[k]);
		}
		free(tmp_datdir);
	}
}

