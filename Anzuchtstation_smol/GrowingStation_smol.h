/*
* DESCRIPTION:  Automated growing station for plants that can send messages via telegram, using an ESP32 or ESP8266 (smaller version, cause old version doesn't work xD).
*               Under "Bot commands" are commands that can be sent over telegram to do certain tasks (detailed description beneath them.
*       Usable sensors/actors:
*         - soil sensor
*         - DHT-Sensor (humidity, temperature)
*         - water pump
*         -
*
 * Author: Mika Angeli
 */
#include "DHTesp.h"
#include <UniversalTelegramBot.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>


#define DHT_ERROR -273

typedef const int TG_SENDING;
TG_SENDING SUCCESS = 1;
TG_SENDING SENDING_ERROR = -1;
TG_SENDING INVALID_USER = -2;
TG_SENDING BOT_NOT_SETUP = -3;
TG_SENDING WIFI_ERROR = -4;
TG_SENDING LOOPING_ERROR = -5;


// #---# Bot commands #---#
  // _ general system infos "G0xx" _
#define CMD_HELP "G000" // Send list of commands to telegram user
  // _ get station information "G1xx" _
#define CMD_GETMEASUREMENTS "G100"  //send air-temperature, air-humidity and soil-humidity
// _ telegram informations "G2xx" _
#define CMD_USERADD "G201"  //add a telegram-ChatID "G201 >String chatID<"
#define CMD_USERRETURN "G202" //return all the telegram-ChatIDs
// _ set values to command followed by number "G5xx + xxxxx" _
#define CMD_SETDHTDELAY "G500"
#define CMD_SETLOOPDELAY "G501"
#define CMD_SETPUMPDELAY "G502"
#define CMD_SETHUMITRESH "G503"
#define CMD_SETSENDDELAY "G504"
#define CMD_SETDEBUG "G505"


class GrowingStation_smol {
private:

	// #---# SENSORS #---#
	// #- various methods, functions and variables for using the DHT-sensor -#
	unsigned long _lastDhtRead = 0;
#define DHT11_SAMPLING 1000;
#define DHT22_SAMPLING 2000;
	void _dhtDelay();


	// #---# Commands #---#
	// #- various methods, functions and variables for the telegram commands -#
	String _commandSTD = "XXXXX";
	String _command;
	String _param1;
	String _param2;
	String _param3;
#define COMMAND_MAX 4
	String* _fullCommand[COMMAND_MAX];

	void _resetCommandArray();
	int _cropCommand(String fullCommand, int lastSpace = 0, int loop = 0);




public:

	// +--+ system +--+
	bool debug = true;


	// +--+ delays (most of them in milliseconds) +--+
	unsigned long dht_sampling = DHT11_SAMPLING; // - delay between dht reading -
	unsigned long loop_delay = 5000; // - delay between entire loops -
	unsigned int watering_frequencyH = 24; // - how often it's watered (in hours) -
	unsigned long watering_amount = 5000; // - how long the pump is activated for watering
	unsigned long watering_treshold = 2000; // - on what soilHumidity the pump will be activated -

	// +--+ wifi +--+
	String wiFi_ssid;

	// +--+ telegram bot +--+
	UniversalTelegramBot* telegramBot = NULL;


	// +--+ users +--+
	String userSTD = "XXXXXXXXX";
	String user1;
	String user2;
	String user3;
	String user4;
	String user5;
#define USER_MAX 5
	String* users[USER_MAX];
	unsigned int userCount = 0;


	// +--+ measurements +--+
	int temperature = 0;
	unsigned int humidity = 0;
	unsigned int soilHumidity = 0;
#define ANALOG_MAX_SAMPLES 255
	int analogReads[ANALOG_MAX_SAMPLES];
	unsigned int analogSamples = 16;

	// #---# WIFI #---#
	// #- various methods and functions for the WiFi -#
	void wifiConnect(const char* ssid, const char* pswd);
	bool wifiConnected();

	// #---# TELEGRAM BOT #---#
	// #- various methods and functions for using the telegram bot -#
	void tg_begin(const String newBotToken, String chatID);
	TG_SENDING tg_send(String text, String* chatID = NULL, unsigned int reps = 5);
	bool tg_send(const char* text, String* chatID = NULL, unsigned int reps = 5);
	void tg_handleMessages(unsigned int messageCount = 1);
	bool tg_isChatID(String chatID);


	// #---# SENSORS #---#
	// #- various methods and functions for using the DHT-sensor -#
	int dht_readTemperature(DHTesp dht);
	int dht_readHumidity(DHTesp dht);
	int analogReadSmooth(unsigned int pin, unsigned int samples = analogSamples);


	// #---# DEBUGGING/SERIAL #---#
	// #- various methods and functions about serial port checks, serial printing usw. -#
	bool debugAvailable();
	bool debugPrintln(const char* text);
	bool debugPrintln(int number);
	bool debugPrintln(String text);

	bool debugPrint(const char* text);
	bool debugPrint(int number);
	bool debugPrint(String text);


	// #---# MISCELLANEOUS #---#
	// #- various methods and functions about serial port checks, serial printing usw. -#
	float millisToHours(unsigned long millis);
	int averageArray(int values[], int size);
};
