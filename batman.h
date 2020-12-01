#ifndef __BATMAN__
#define __BATMAN__


#define INTERVAL 60
#define BUFFSIZE 256


#define POWER_SUPPLY_DIR "/sys/class/power_supply/"
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



void error(char *report);

void display_usage();

char *read_file_line( char *filename, char *read_data_buffer );

void write_file_line( char *filename, double data );

void append_file_line( char *filename, double data );

void get_power_modes( char *power_modes[] );

void display_info();

void display_stats();

double calc_product( double num1, double num2 );

double calc_ratio( double num1, double num2 );

int get_proc_id_by_name( char *proc_name );

int get_ppid_by_pid( const pid_t pid );

void batman_daemon();

void batman_daemon_detective();

void task_master( int start_daemon, int stop_daemon );

void display_notifications( char *title_name, char *title_report, int URGENCY, char *caution_report, const char *icon_pathname );


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
