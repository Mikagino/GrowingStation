/* ################################################################################
 * #---# GPIOS #---#
 *
 * General purpose input output pin declaration
 * ################################################################################
*/



// # Input-/Output-Pins in struct for better overview #
struct GPIO {
	const int waterPump = 23;
	//const int soilSensor = 35;
	// - DHT temp/humi sensors -
//#define SOIL_PIN 13
#define DHT_PIN 32
};





/* ################################################################################
 * #---# WATERING #---#
 *
 * Configure what features you wanna use for watering
 * ################################################################################
*/


/* Soil sensor:
* Analog soil humidity sensor will be used for watering (set to -1 to deactivate)
*/
#define SOIL_SENSOR
#ifdef SOIL_SENSOR
#define SOIL_SENSOR_INPUT1 13
#define SOIL_SENSOR_INPUT2 -1
#define SOIL_SENSOR_INPUT3 -1
#endif // SOIL_SENSOR



/* Manual watering:
 * Station will be watered on input from MANUAL_WATERING_BUTTON
*/
#define MANUAL_WATERING
#ifdef MANUAL_WATERING
#define MANUAL_WATERING_INPUT 35
#endif // MANUAL_WATERING





/* ################################################################################
 * #---# MESSAGES #---#
 *
 * All messages for the bot
 * ################################################################################
*/



struct Messages {
	const String startup = "Anzuchtstation_smol rebooted...";
	const String pumped = "* Water pumped *";
};