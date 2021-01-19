#ifndef __BATMAN__
#define __BATMAN__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>           // reading from user database
#include <signal.h>           // getting directory names and framworks.
#include <pwd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdint.h>
#include <getopt.h>             // Parsing command-line arguments
#include <syslog.h>

#include <libnotify/notify.h>
#include <ao/ao.h>
#include <mpg123.h>


#define INTERVAL 60
#define BUFFSIZE 128
#define BITS 8


#define POWER_SUPPLY_DIR "/sys/class/power_supply/"
#define VAR_WORK_DIR "batman/"

#define PS_CAPACITY_LEVEL "capacity_level"
#define PS_CAPACITY "capacity"
#define PS_CHARGE_FULL "charge_full"
#define PS_CHARGE_FULL_DESIGN "charge_full_design"
#define PS_CHARGE_NOW "charge_now"
#define PS_CURRENT_NOW "current_now"
#define PS_VOLTAGE_MIN_DESIGN "voltage_min_design"
#define PS_VOLTAGE_NOW "voltage_now"
#define PS_STATUS "status"
#define PS_PRESENT "present"
#define PS_TYPE "type"
#define PS_TECHNOLOGY "technology"
#define PS_SERIAL_NUMBER "serial_number"
#define PS_MODEL_NAME "model_name"
#define PS_MANUFACTURER "manufacturer"
#define PS_ONLINE "online"
#define PS_ALARM "alarm"


/*---------------------------------------------------- Actions ----------------------------------------------*/

void errorlog(char *report);
void write_file_line( char *filename, double data );
void write_file_line_as_char( char *filename, char *data );
void append_file_line( char *filename, double data );
void append_file_line_as_char( char *filename, char *data );
void get_power_modes( char *power_modes[] );
void display_info();
void display_stats();
void monitor_events();
void batman_daemon();
void batman_daemon_detective();
void play_sound_effects( const char *sound_effect );
void display_notifications( NotifyNotification *notify_batman, GError *error, char *PS_NAME, char *header, u_int8_t URGENCY, char *caution_report, const char *icon_pathname );


/*---------------------------------------------------- Methods ----------------------------------------------*/

char *read_file_line( char *filename, char *read_data_buffer );
char *read_file_content( char *filename, char *read_data_content );
char *get_home_dir();
double calc_product( double num1, double num2 );
double calc_ratio( double num1, double num2 );
int get_proc_id_by_name( char *proc_name );
int get_ppid_by_pid( const int pid );
int pid_has_tty( int pid );
int verify_cmdline( int pid, char *cmdline, int char_length );


/*------------------------------------------------- Color Scheme ------------------------------------------*/

char *red();
char *redb();
char *green();
char *greenb();
char *yellow();
char *yellowb();
char *blue();
char *blueb();
char *magenta();
char *magentab();
char *cyan();
char *cyanb();
char *resetc();


#endif // __BATMAN__
