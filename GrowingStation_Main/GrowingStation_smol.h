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
#include "Commands.h"
#include <Arduino.h>
#ifndef CONFIG
#define CONFIG
#include "Config.h"
#endif // CONFIG

#ifndef MPD
#define MPD
#include "MP_Delay.h"
#endif



class GrowingStation_smol {
private:

	// #---# DHT-SENSOR #---#
	void _dht_delay();
	MP_Delay mpd_dht;


#ifdef TELEGRAM_BOT
	// #---# TELEGRAM-BOT #---#
	char _currentChatID[USER_LENGTH];
	String _currentChatName;
	String _currentChatText;
	bool _currentChatPermission;
	String _debugTgMsg;
	String _sendingText;
	char _fullCommand[COMMAND_LENGTH][4];

	CMD_STATUS _cutCommand(String cmd, unsigned int loop = 0);
	unsigned int _commandLength = 0;
	//void _setupCommands();


	// #---# WIFI #---#
	const char* _wifi_ssid;
	const char* _wifi_pswd;
#endif





public:

	// #---# SYSTEM #---#
	bool debug = true;


	// #---# DELAYS #---#
	unsigned long dht_sampling = DHT11_SAMPLING; // - delay between dht reading (in ms) -
	unsigned long loop_delay = 5000; // - delay between entire loops (in ms) -
	unsigned int watering_frequency_H = 72; // - how often it's watered (in hours) -
	unsigned int lastPump_H = 0; // - on what RTC-time the pump was last activated (in hours) -
	unsigned long watering_amount = 15000; // - how long the pump is activated for watering (in ms) -
	unsigned long watering_treshold = 2000; // - on what soilHumidity the pump will be activated (mV) -


#ifdef TELEGRAM_BOT
	// #---# TELEGRAM-BOT #---#MANUAL_WATERING
	UniversalTelegramBot* telegramBot = NULL;
	bool wifiConnected = false;


	// #---# USERS #---#
	unsigned int userCount = 0;
#define userSTD "XXXXXXXXX"
	char users[USER_MAX][USER_LENGTH];
	bool usersWritePermissions[USER_MAX];
#endif


	// #---# MEASUREMENTS #---#
	int temperature = 0;
	unsigned int humidity = 0;
	int soilHumidity = 0;


	// #---# PERIPHERALS #---#
	void pumpWater(unsigned int pumpIndex = ALL_PUMPS);


#ifdef TELEGRAM_BOT
	// #---# TELEGRAM BOT #---#
	void tg_begin(UniversalTelegramBot* bot, char chatID[USER_LENGTH]);
	void assignUser(char chatID[USER_LENGTH], int userIndex, bool writePermission = false);
	TG_SENDING tg_send(String text, char chatID[USER_LENGTH] = NULL, unsigned int reps = 5);
	TG_SENDING tg_send(const char* text, char chatID[USER_LENGTH] = NULL, unsigned int reps = 5);
	void tg_handleMessages();
	int tg_checkChatID(char chatID[9]);

	// #---# WIFI #---#
	bool wifiConnect(const char* ssid = NULL, const char* pswd = NULL);
	bool wifiCheck(int iterations = 1);
	void wifiRestart(WIFI_RESTART_MODE mode);
#else
	void begin();
#endif


	// #---# DHT-SENSOR #---#
	int dht_readTemperature(DHTesp dht);
	int dht_readHumidity(DHTesp dht);


	// #---# DEBUGGING/SERIAL #---#
	bool debugAvailable(String text);
	bool debugPrintln(const char* text);
	bool debugPrintln(int number);
	bool debugPrintln(String text);

	bool debugPrint(const char* text);
	bool debugPrint(int number);
	bool debugPrint(String text);


	// #---# MISCELLANEOUS #---#
	float millisToHours(unsigned long millis);
	bool isBool(char value[COMMAND_LENGTH]);
	bool toBool(char value[COMMAND_LENGTH]);
};
