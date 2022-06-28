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
#include "Config.h"
#include "Commands.h"

#define DHT_ERROR -273
#define ALL_PUMPS -272

typedef const int TG_SENDING;
TG_SENDING SUCCESS = 1;
TG_SENDING SENDING_ERROR = -1;
TG_SENDING INVALID_USER = -2;
TG_SENDING BOT_NOT_SETUP = -3;

typedef const int CMD_STATUS;
CMD_STATUS GOOD = 1;
CMD_STATUS PARAM_ERROR = -1;
CMD_STATUS FUNCTION_STOP = -2;



class GrowingStation_smol {
private:

	// #---# DHT-SENSOR #---#
	// #- various methods, functions and variables for using the DHT-sensor -#
	unsigned long _lastDhtRead = 0;
#define DHT11_SAMPLING 1000;
#define DHT22_SAMPLING 2000;
	void _dhtDelay();


	// #---# Commands #---#
	// #- various methods, functions and variables for the telegram commands -#
	String _commandSTD = "XXXXXXXXXXXXXXXXXXXX";
	String _paramSTD = "XXXXX";
	String _command;
	String _param1;
	String _param2;
	String _param3;
#define COMMAND_MAX 4
	String* _fullCommand[COMMAND_MAX];

	void _resetCommand();
	CMD_STATUS _cutCommand(String cmd, unsigned int loop = 0);
	//void _setupCommands();




public:

	// +--+ system +--+
	bool debug = true;


	// +--+ delays (most of them in milliseconds) +--+
	unsigned long dht_sampling = DHT11_SAMPLING; // - delay between dht reading -
	unsigned long loop_delay = 5000; // - delay between entire loops (in ms) -
	unsigned int watering_frequency_H = 72; // - how often it's watered (in hours) -
	unsigned long watering_amount = 15000; // - how long the pump is activated for watering (in ms) -
	unsigned long watering_treshold = 2000; // - on what soilHumidity the pump will be activated -


	// +--+ telegram bot +--+
	UniversalTelegramBot* telegramBot = NULL;
	bool wifiConnected = false;


	// +--+ users +--+
#define USER_MAX 5
	unsigned int userCount = 0;

	String userSTD = "XXXXXXXXX";
	String user1;
	String user2;
	String user3;
	String user4;
	String user5;
	String* users[USER_MAX];

	bool user1_Write;
	bool user2_Write;
	bool user3_Write;
	bool user4_Write;
	bool user5_Write;
	bool* usersWrite[USER_MAX];


	// +--+ measurements +--+
	int temperature = 0;
	unsigned int humidity = 0;
	int soilHumidity = 0;

	// #---# STATION CONTROL #---#
	// #- various methods and functions for controlling the station -#
	void pumpWater(int pumpIndex = ALL_PUMPS);


	// #---# TELEGRAM BOT #---#
	// #- various methods and functions for using the telegram bot -#
	void tg_begin(UniversalTelegramBot* bot, String chatID);
	TG_SENDING tg_send(String text, String* chatID = NULL, unsigned int reps = 5);
	void wifiConnect(const char* ssid, const char* pswd);
	bool tg_send(const char* text, String* chatID = NULL, unsigned int reps = 5);
	void tg_handleMessages();
	bool tg_isChatID(String chatID);
	int tg_getChatID(String chatID);


	// #---# DHT-SENSOR #---#
	// #- various methods and functions for using the DHT-sensor -#
	int dht_readTemperature(DHTesp dht);
	int dht_readHumidity(DHTesp dht);


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
	bool isBool(String string);
	bool toBool(String string);
};
