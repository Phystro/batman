#ifndef __BATMAN__
#define __BATMAN__

#define BUFFSIZE 8192

#define POWER_SUPPLY_DIR "/sys/class/power_supply/"
#define PS_CAPACITY_LEVEL "capacity_level"
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


void error(char *report);
char *getHomeDir();

#endif //__BATMAN__
