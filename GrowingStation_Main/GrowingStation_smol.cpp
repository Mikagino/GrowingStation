#include "GrowingStation_smol.h"
#include <arduino-timer.h>
//#include "Config.h"




/* ################################################################################
*
 * #---# Startup #---#
 *
 * ################################################################################
*/


#ifndef TELEGRAM_BOT
void GrowingStation_smol::begin() {
	Serial.begin(9600);
}
#endif // !TELEGRAM_BOT





/* ################################################################################
*
 * #---# STATION PERIPHERALS #---#
 *
 * ################################################################################
*/


/**
 *  DESCRIPTION: Send messages over telegram, serial and pump water for watering_amount milliseconds
 *
 *  PARAM:
 *    pumpIndex -> index of the pump that will be activated (use no parameter, to pump on all)
 *	  pumpMode -> just changes debugging messages (MANUAL_PUMP, AUTO_PUMP)
 *	  pumpPin -> don't give parameter, is used for
 *///TODO: make it multithreaded (each one on and off after watering_amount)
void GrowingStation_smol::pumpWater(unsigned int pumpIndex) {
	if (pumpIndex == ALL_PUMPS) {
		for (int i = 0; i < SIZEOF_ARRAY(WATER_PUMP_PINS); i++) {
			pumpWater(i);
		}
	}
	else {
		if (pumpIndex <= 9) {
			lastPump_H = millisToHours(millis());
			digitalWrite(WATER_PUMP_PINS[pumpIndex], 1);
			delay(watering_amount);
			digitalWrite(WATER_PUMP_PINS[pumpIndex], 0);
			char text[SIZEOF_ARRAY(MSG_PUMPED) + 1] = MSG_PUMPED;
			text[SIZEOF_ARRAY(MSG_PUMPED)] = (char)(pumpIndex + 49);
			debugPrintln(text);
			String sendingText = String(text);
			tg_send(sendingText, users[0]);
		}
		else {
			char text[] = "ERROR: Invalid index selected for pump!";
			debugPrintln(text);
			tg_send(text);
		}
	}
	//wateringMode = &AUTO_WATERING_MODE;
}





/* ################################################################################
*
 * #---# TELEGRAM BOT #---#
 *
 * ################################################################################
*/


#ifdef TELEGRAM_BOT

/**
 *  DESCRIPTION: setup serial output on Baudrate 9600, assign user/command pointer array to variables
 *
 *  PARAM:
 *    @botToken -> the token of the telegram bot (got from botfather on telegram, when creating bot)
 *    @chatID -> first user-id (got from ID-bot on telegram)
 */
void GrowingStation_smol::tg_begin(UniversalTelegramBot* bot, char chatID[USER_LENGTH]) {
	telegramBot = bot;

	for (int i = 1; i < USER_MAX; i++) {
		usersWritePermissions[i] = false;
	}

	assignUser(chatID, 0, true);
	userCount++;

	//_setupCommands();

	Serial.begin(9600);
}



/**
 *  DESCRIPTION: Store a new chatID in the users-array
 *
 *  PARAM:
 *    chatID -> chatID of telegram user
 *    userIndex -> index in the array, the user will be stored to
 *    writePermission -> set writing permission for the user
 */
void GrowingStation_smol::assignUser(char chatID[USER_LENGTH], int userIndex, bool writePermission) {
	for (int i = 0; i < USER_LENGTH; i++) {
		users[userIndex][i] = chatID[i];
	}
	usersWritePermissions[userIndex] = writePermission;
}



/**
 *  DESCRIPTION: Try sending the message reps times with the universal telegram bot to an specific chatID or to all, when chatID is NULL
 *
 *  PARAM:
 *    text -> the text you want to send
 *    chatID -> telegram user-ID the message will be sent to
 *    reps -> repititions of looping through sending
 *
 *    RETURN:
 *		 1 = SUCCESS
 *		 2 = SENDING_ERROR
 *		 3 = INVALID_USER
 *		 4 = BOT_NOT_SETUP
 */
TG_SENDING GrowingStation_smol::tg_send(String text, char chatID[USER_LENGTH], unsigned int reps) {
	if (telegramBot == NULL) {
		return BOT_NOT_SETUP;
	}
	else {
		if (chatID != ALL_USERS) { // -> try sending reps times to one user	
			for (int i = 0; i < reps; i++) {
				if (strcmp(chatID, userSTD) && telegramBot->sendMessage(String(chatID), text)) {
					debugPrint("==> '"); debugPrint(text); debugPrint("'"); debugPrint(" successfully sent to: "); debugPrintln(chatID);
					return SUCCESS;
				}
				else {
					return INVALID_USER;
				}
			}
		}
		else { // -> try sending to all users reps times
			int msgSent = 0;
			for (int j = 0; j < USER_MAX; j++) {
				if (strcmp(users[j], userSTD)) {
					continue;
				}
				else if (tg_send(text, users[j]) == SUCCESS) {
					msgSent++;
				}
			}

			if (msgSent > 0) {
				debugPrint("==> '"); debugPrint(text); debugPrint("'"); debugPrint(" successfully sent to: "); debugPrint(msgSent); debugPrintln(" users");
				return SUCCESS;
			}
		}
	}
	return SENDING_ERROR;
}

TG_SENDING GrowingStation_smol::tg_send(const char* text, char chatID[USER_LENGTH], unsigned int reps) {
	return tg_send(String(text), chatID, reps);
}



/**
 * DESCRIPTION: Handle messages in telegram chat and respond (commands described in "Commands.h")
 *
 * PARAM:
 *		chatID -> pointer to the chat-ID you want to read from (default: user1)
 *		messageCount -> count of messages you want to read
 */
void GrowingStation_smol::tg_handleMessages() {
	int msgNum = telegramBot->getUpdates(telegramBot->last_message_received + 1);
	if (telegramBot != NULL) {
		for (int i = 0; i < msgNum; i++) {
			(telegramBot->messages[i].chat_id).toCharArray(_currentChatID, USER_LENGTH);
			_currentChatName = (telegramBot->messages[i].from_name);
			_currentChatText = (telegramBot->messages[i].text);
			_currentChatPermission = (usersWritePermissions[tg_checkChatID(_currentChatID)]);

			_debugTgMsg = "'" + _currentChatText + "' entered by " + _currentChatName;
			debugPrintln(_debugTgMsg);

			if (tg_checkChatID(_currentChatID) != -1) {
				if (_currentChatText.equals("/start")) {
					_sendingText = String("Hello, I am GrowingStation!\n");
					_sendingText += String("Your automatic station to water your plants and read its sensor data from anywhere.\n\n");
					_sendingText += String("Type ") + CMD_HELP + String(" to get all the commands...");
				}
				// - System info commands -
				else if (_currentChatText.charAt(1) == SYSTEM_INFOS && _currentChatText.length() == 4) {
					if (_currentChatText.equals(CMD_HELP)) {
						_sendingText = String("## COMMANDS ##");
						_sendingText += String("\n\nSystem Informations");
						_sendingText += String("\n  Help: ") + CMD_HELP;
						_sendingText += String("\n\nGetters");
						_sendingText += String("\n  Temperature: ") + CMD_GET_TEMP;
						_sendingText += String("\n  Humidity: ") + CMD_GET_HUMI;
						_sendingText += String("\n\nSetters");
						_sendingText += String("\n  DHT-Sampling: ") + CMD_SETDHTSAMPLING + " [0000]ms";
						_sendingText += String("\n  Loop-Delay: ") + CMD_SETLOOPDELAY + " [0000]ms";
						_sendingText += String("\n  Watering-Amount: ") + CMD_SETWATERINGAMOUNT + " [0000]ms";
						_sendingText += String("\n  Watering-Frequency: ") + CMD_WATERINGFREQUENCY + " [0000]h";
						_sendingText += String("\n  Humidity-Treshold: ") + CMD_SETHUMITRESH + " [0000]mV";
						_sendingText += String("\n  Debugging: ") + CMD_SETDEBUG + " [1:true/0:false]";
					}
				}

				// - Getter commands -
				else if (_currentChatText.charAt(1) == GETTER && _currentChatText.length() == 4) {
					if (_currentChatText.equals(CMD_GET_TEMP)) {
						_sendingText = String("Temperature: ") + temperature + "C";
					}
					else if (_currentChatText.equals(CMD_GET_HUMI)) {
						_sendingText = String("Humidity: ") + humidity + "%";
					}

				}
				// - Setter commands -
				else if (_currentChatText.charAt(1) == SETTER && _currentChatText.length() > 4) {
					if (!_currentChatPermission) {
						_sendingText = String("You don't have the permission to change variables!");
						_sendingText += String("\nAsk an admin to change your writing permission\n");
						_sendingText += String("Use ") + CMD_GETUSERS + " to get all users(with permission info)";
					}
					else {
						_cutCommand(_currentChatText);
						// # DHT-sampling #
						if (_currentChatText.equals(CMD_SETDHTSAMPLING)) {
							int value = atoi(_fullCommand[CMD_PARAM_1]);
							if (value != 0) {
								dht_sampling = value;
								_sendingText = String("dht_sampling set to ") + dht_sampling;
							}
							else {
								_sendingText = String("ERROR: ") + _fullCommand[CMD_PARAM_1] + " is an invalid parameter!";
							}
						}
						// # Loop-Delay #
						else if (_currentChatText.equals(CMD_SETLOOPDELAY)) {
							int value = atoi(_fullCommand[CMD_PARAM_1]);
							if (value != 0) {
								loop_delay = value;
								_sendingText = String("loop_delay set to ") + loop_delay;
							}
							else {
								_sendingText = String("ERROR: ") + _fullCommand[CMD_PARAM_1] + " is an invalid parameter!";
							}
						}
						// # Debugging #
						else if (_currentChatText.equals(CMD_SETDEBUG)) {
							if (isBool(_fullCommand[CMD_PARAM_1])) {
								debug = toBool(_fullCommand[CMD_PARAM_1]);
								_sendingText = String("debug set to ") + debug;
							}
							else {
								_sendingText = String("ERROR: ") + _fullCommand[CMD_PARAM_1] + " is an invalid parameter!";
							}
						}
					}
				}
				else if (_currentChatText.equals(KEK)) {
					_sendingText = String("You're a kek yourself! ;^;");
				}
				else {
					_sendingText = String("ERROR: ") + _currentChatText + " is an invalid command!";
				}
				tg_send(_sendingText, _currentChatID);
			}
			// - Command error -
			else {
				String _sendingText = String("ERROR: ") + "You're an invalid user, " + _currentChatName + "!";
				tg_send(_sendingText, _currentChatID);
			}
		}
	}

}



/**
 * DESCRIPTION: Check if chatID is in users[x]
 *
 * PARAM:
 *		chatID -> chatID that will be checked
 *
 * RETURN:
 *		-1 -> chat-ID is not a saved user
 *		int -> Array index of users (read and write rights are saved in userWrite[x])
 */
int GrowingStation_smol::tg_checkChatID(char chatID[9]) {
	for (int i = 0; i < USER_MAX; i++) {
		if (strcmp(chatID, users[i])) {
			return i;
		}
	}
	return -1;
}



/** # PRIVATE #
* DESCRIPTION: setup all the commands for the bot
*/
//void GrowingStation_smol::_setupCommands() {
//	const String cmds = F("["
//		"{\"command\":\"help\",  \"description\":\"Get bot usage help\"},"
//		"{\"command\":\"start\", \"description\":\"Message sent when you open a chat with a bot\"},"
//		"{\"command\":\"status\",\"description\":\"Answer device current status\"}" // no comma on last command
//		"]");
//	/*const String cmds = F("["
//		"{\"command\":\"start\", \"description\":\"Startup the bot\"},"
//		"{\"command\":\"help\", \"description\":\"Get info on all commands\"},"
//		"{\"command\":\"temp\", \"description\":\"Get current air temperature\"},"
//		"{\"command\":\"humi\", \"description\":\"Get current air humidity\"},"
//		"{\"command\":\"getusers\", \"description\":\"Get all the users with their name\"},"
//		"{\"command\":\"setuser\", \"description\":\"Set a new user (returns ERROR, when list is full)\"}"
//		"]");*/
//	telegramBot->setMyCommands(cmds);
//}





/*################################################################################
*
 * #---# WIFI #---#
 *
 *################################################################################
*/


/**
 *  DESCRIPTION: Try connecting to the WiFi 10 times
 *
 *  PARAM:
 *    ssid -> name of the wifi
 *	  pswd -> password of the wifi
 */
bool GrowingStation_smol::wifiConnect(const char* ssid, const char* pswd) {
	if (ssid != NULL && pswd != NULL) {
		_wifi_ssid = ssid;
		_wifi_pswd = pswd;
	}
	if (_wifi_ssid == NULL || _wifi_pswd == NULL) {
		return false;
	}

	WiFi.begin(_wifi_ssid, _wifi_pswd);
	return wifiCheck(10);
}



/**
 *  DESCRIPTION: Check if the WiFi is connected and restart after iterations tries
 */
bool GrowingStation_smol::wifiCheck(int iterations) {
	for (int i = 1; i <= iterations; i++) {
		if (WiFi.status() != WL_CONNECTED) {
			debugPrintln("WiFi try connecting...");
			delay(1000);
		}
		else {
			debugPrintln("WiFi is connected!");
			return true;
		}
	}

	debugPrintln("ERROR: Can't connect to WiFi!\nReconnecting to WiFi...");
	WiFi.disconnect();
	WiFi.reconnect();
	return wifiCheck(10);
}



/**
 *  DESCRIPTION: Restart the wifi via esp.restart() or wifi.disconnect()
 *
 * PARAM:
 *	mode -> mode of restarting (WIFI_RESTART or ESP_RESTART)
 */
void GrowingStation_smol::wifiRestart(WIFI_RESTART_MODE mode) {
	if (mode == WIFI_RESTART) {
		WiFi.disconnect(true);
		debugPrintln("WiFi restarting...\n\n");
		delay(1000);
		wifiConnect();
	}
	else if (mode == ESP_RESTART) {
		ESP.restart();
	}
}





/*################################################################################
*
 * #---# Telegram Commands #---#
 *
 *################################################################################
*/


/** # PRIVATE #
* DESCRIPTION: Stores the command and the params into the _fullCommand String array
*
* RETURN:
*	PARAM_ERROR -> too many params
*	FUNCTION_ERROR -> function stopped neutral
*	GOOD -> command cut properly
*/
CMD_STATUS GrowingStation_smol::_cutCommand(String cmd, unsigned int loop) {
	unsigned int cmdLength = cmd.length();
	if (loop > 3 || cmdLength >= COMMAND_LENGTH) {
		return PARAM_ERROR;
	}
	else if (cmdLength != 0) {
		for (unsigned int i = 0; i < cmdLength; i++) {
			if (cmd.charAt(i) == ' ') {
				(cmd.substring(0, i)).toCharArray(_fullCommand[loop], COMMAND_LENGTH);
				_cutCommand(cmd.substring(i, cmdLength), loop++);
			}
		}
		_commandLength = loop;
		return GOOD;
	}
	return FUNCTION_ERROR;
}

#endif // TELEGRAM_BOT





/*################################################################################
*
 * #---# DHT-SENSOR #---#
 *
 *################################################################################
*/


/**
 * DESCRIPTION: read temperature with an DHT11-sensor and store it to the private variable temperature
 * PARAM:
 *    dht -> the dht-object you want to measure with ( setup with "dht.setup(DHT_PIN, DHTesp::DHT11)" )
 *
 *  RETURN:
 *   int -> temperature rounded to int
 *   DHT_ERROR -> on sensor failure
 */
int GrowingStation_smol::dht_readTemperature(DHTesp dht) {
	_dht_delay();
	temperature = round(dht.getTemperature());

	if (dht.getStatus() != 0) {
		return DHT_ERROR;
	}
	else {
		return temperature;
	}
}



/**
 *  DESCRIPTION: read humidity with an DHT11-sensor and store it to the private variable humidity
 *
 *  PARAM:
 *    dht -> the dht-object you want to measure with ( setup with "dht.setup(DHT_PIN, DHTesp::DHT11)" )
 *
 *  RETURN:
 *   int -> humidity rounded to int
 *   DHT_ERROR -> on sensor failure
 */
int GrowingStation_smol::dht_readHumidity(DHTesp dht) {
	_dht_delay();
	humidity = dht.getHumidity();

	if (dht.getStatus() != 0) {
		return DHT_ERROR;
	}
	else {
		return humidity;
	}
}



/**
 *  DESCRIPTION: loop until the dht is allowed to measure
 */
void GrowingStation_smol::_dht_delay() {
	while (!mpd_dht.delay(dht_sampling)) {
		delay(50);
	}
	/*while (_lastDhtRead + dht_sampling > millis) {
		delay(50);
	}
	_lastDhtRead = millis;*/
}





/*################################################################################
 *
 * #---# DEBUGGING / SERIAL #---#
 *
 *################################################################################
*/


/**
 *  DESCRIPTION: check if debugging is enabled and Serial port is available
 *
 *  RETURN:
 *   true -> Serial port is accessable
 *   false -> Serial port is unaccessable
 */
bool GrowingStation_smol::debugAvailable(String text) {
	if (Serial.availableForWrite() > text.length() && debug) {
		return true;
	}
	else {
		return false;
	}
}



/**
 * DESCRIPTION: Send println() message over serial port  (baudrate: 9600) when it's available and debugging is enabled
 *
 *  PARAM:
 *    text -> debugging message to be sent over serial port
 *
 *  RETURN:
 *   true -> serial message sent
 *   false -> serial message not sent
 */
bool GrowingStation_smol::debugPrintln(const char* text) {
	if (debugAvailable(String(text))) {
		Serial.println(text);
		return true;
	}
	else {
		return false;
	}
}

/**
 * DESCRIPTION: Send println() message over serial port  (baudrate: 9600) when it's available and debugging is enabled
 *
 *  PARAM:
 *    text -> debugging message to be sent over serial port
 *
 *  RETURN:
 *   true -> serial message sent
 *   false -> serial message not sent
 */
bool GrowingStation_smol::debugPrintln(int number) {
	char buffer[16];
	itoa(number, buffer, 10);
	return debugPrintln(buffer);
}

/**
 * DESCRIPTION: Send println() message over serial port  (baudrate: 9600) when it's available and debugging is enabled
 *
 *  PARAM:
 *    text -> debugging message to be sent over serial port
 *
 *  RETURN:
 *   true -> serial message sent
 *   false -> serial message not sent
 */
bool GrowingStation_smol::debugPrintln(String text) {
	return debugPrintln(text.c_str());
}



/**
 *  DESCRIPTION: Send print() message over serial port (baudrate: 9600) when it's available and debugging is enabled
 *
 *  PARAM:
 *    text -> debugging message to be sent over serial port
 *
 *  RETURN:
 *   true -> serial message sent
 *   false -> serial message not sent
 */
bool GrowingStation_smol::debugPrint(const char* text) {
	if (debugAvailable(String(text))) {
		Serial.print(text);
		return true;
	}
	else {
		return false;
	}
}

/**
 *  DESCRIPTION: Send print() message over serial port (baudrate: 9600) when it's available and debugging is enabled
 *
 *  PARAM:
 *    text -> debugging message to be sent over serial port
 *
 *  RETURN:
 *   true -> serial message sent
 *   false -> serial message not sent
 */
bool GrowingStation_smol::debugPrint(int number) {
	char buffer[16];
	itoa(number, buffer, 10);
	return debugPrint(buffer);
}

/**
 *  DESCRIPTION: Send print() message over serial port (baudrate: 9600) when it's available and debugging is enabled
 *
 *  PARAM:
 *    text -> debugging message to be sent over serial port
 *
 *  RETURN:
 *   true -> serial message sent
 *   false -> serial message not sent
 */
bool GrowingStation_smol::debugPrint(String text) {
	return debugPrint(text.c_str());
}





/*################################################################################
*
 * #---# MISCELLANEOUS #---#
 *
 *################################################################################
*/


/**
 *  DESCRIPTION: convert milliseconds to  hours
 */
float GrowingStation_smol::millisToHours(unsigned long millis) {
	return millis / 1000 / 60 / 60;
}



/**
* DESCRIPTION: Check if string is a boolean
*/
bool GrowingStation_smol::isBool(char value[COMMAND_LENGTH]) {
	if (atoi(value) == 1 || atoi(value) == 0) {
		return true;
	}
	return false;
}



/**
* DESCRIPTION: Return boolean value of String
*/
bool GrowingStation_smol::toBool(char value[COMMAND_LENGTH]) {
	if (atoi(value) == 1) {
		return true;
	}
	return false;
}