#include <cstdint>



#ifndef CONFIGS
#define CONFIGS





/* ################################################################################
 *
 * #---# WIFI #---#
 *
 * ################################################################################
*/


// - base config -
#define WIFI_SSID "Martin-Router-King"
#define WIFI_PASSWORD "vorlaeufigespw"

// - typedefs -
typedef const uint8_t WIFI_RESTART_MODE;
WIFI_RESTART_MODE ESP_RESTART = 0;
WIFI_RESTART_MODE WIFI_RESTART = 1;





/* ################################################################################
 *
 * #---# TELEGRAM #---#
 *
 * ################################################################################
*/


// - Messsages -
const char MSG_STARTUP[] = "Anzuchtstation_smol rebooted...";
//const char MSG_PUMPED[] = "Water pumped on P.";
#define MSG_PUMPED "Water pumped on P."


#define TELEGRAM_BOT
#ifndef TELEGRAM_BOT
#define tg_send(x) ;

#else
#define BOT_TOKEN "2018939049:AAF_p6LaS8e2dG3PaWxHOHGm02soKHh42WQ"

// - user defines -
#define USER_LENGTH 10
#define USER_MAX 5
#define USERNAME_LENGTH 15
#define ALL_USERS NULL

// - typedefs -
typedef const uint8_t TG_SENDING;
TG_SENDING SUCCESS = 1;
TG_SENDING SENDING_ERROR = 2;
TG_SENDING INVALID_USER = 3;
TG_SENDING BOT_NOT_SETUP = 4;

typedef const uint8_t CMD_STATUS;
CMD_STATUS GOOD = 1;
CMD_STATUS PARAM_ERROR = 2;
CMD_STATUS FUNCTION_ERROR = 3;
#endif // TELEGRAM_BOT





/* ################################################################################
 *
 * #---# PERIPHERALS #---#
 *
 * ################################################################################
*/



#define DHT_SENSORS
#ifdef DHT_SENSORS
const int DHT_PINS[] = { 32 };
// - constants -
#define DHT_ERROR -273
#define DHT11_SAMPLING 1000
#define DHT22_SAMPLING 2000
#endif //DHT_SENSORS






/* ################################################################################
 *
 * #---# WATERING #---#
 *
 * ################################################################################
*/


#define ALL_PUMPS 256


#define WATER_PUMPS
#ifdef WATER_PUMPS
const int WATER_PUMP_PINS[] = { 23 };
// - typedefs -
#define AUTO_WATERING_MODE true
#define MANUAL_WATERING_MODE false
#endif // WATER_PUMPS


// #---# Soil sensor #---#
// # Analog soil humidity sensor will be used for watering (set to -1 to deactivate) #
// # Comment following line to deactivate #
#define SOIL_SENSORS
#ifdef SOIL_SENSORS
const int SOIL_SENSOR_PINS[] = { 34 };
#endif // SOIL_SENSORS


// #---# Manual watering #---#
// # Station will be watered on input from MANUAL_WATERING_BUTTON (interrupt) #
#define MANUAL_WATERING
#ifdef MANUAL_WATERING
const int MANUAL_WATERING_INPUT = 33;
#endif // MANUAL_WATERING


#define FLOAT_SWITCH
#ifdef FLOAT_SWITCH
const int FLOAT_SWITCH_INPUT = 34;
#endif // FLOAT_SWITCH





/*################################################################################
*
 * #---# MISCELLANEOUS #---#
 *
 *################################################################################
*/


#define SIZEOF_ARRAY(x) sizeof(x)/sizeof(x[0])





#endif // CONFIGS