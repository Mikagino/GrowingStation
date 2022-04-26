#include "GrowingStation_smol.h"





/* ################################################################################
 * #---# WIFI #---#
 *
 * various methods and functions for the WiFi with an ESP32 or ESP2866
 * ################################################################################
*/


/**
 *  DESCRIPTION: connect to the wifi
 *
 *  PARAM:
 *    @ssid -> name of the wifi
 *	  @pswd -> password of the wifi
 */
void GrowingStation_smol::wifiConnect(const char* ssid, const char* pswd) {
	WiFi.begin(ssid, pswd);
	while (!wifiConnected()) {
		debugPrintln("WiFi try connecting...");
		delay(1000);
	}
	debugPrintln("WiFi connected!");
	wiFi_ssid = ssid;
}



/**
 *  DESCRIPTION: test if you are connected to wifi
 *
 *  PARAM:
 *    @ssid -> name of the wifi
 *	  @pswd -> password of the wifi
 */
bool GrowingStation_smol::wifiConnected() {
	if (WiFi.status() != WL_CONNECTED) {
		return true;
	}
	else {
		return false;
	}
}





/* ################################################################################
 * #---# TELEGRAM BOT #---#
 *
 * various methods and functions for using the telegram bot with an ESP32 or ESP2866
 * ################################################################################
*/


/*
 *  DESCRIPTION: setup the telegram bot, serial output on Baudrate 9600, assign user pointer array to variables
 *
 *  PARAM:
 *    @botToken -> the token of the telegram bot (got from botfather on telegram, when creating bot)
 *    @chatID -> first user-id (got from ID-bot on telegram)
 */
void GrowingStation_smol::tg_begin(const String botToken, String chatID) {
	WiFiClientSecure sClient;
	UniversalTelegramBot bot = UniversalTelegramBot(botToken, sClient);
	telegramBot = &bot;
	sClient.setCACert(TELEGRAM_CERTIFICATE_ROOT);

	users[0] = &user1;
	users[1] = &user2;
	users[2] = &user3;
	users[3] = &user4;
	users[4] = &user5;

	*users[0] = chatID;
	userCount++;
	Serial.begin(9600);

	_fullCommand[0] = &_command;
	_fullCommand[1] = &_param1;
	_fullCommand[2] = &_param2;
	_fullCommand[3] = &_param3;

	// - Connect to wifi -

}





/**
 *  DESCRIPTION: Try sending the message @reps times with the universal telegram bot to an specific user-ID
 *
 *  PARAM:
 *    @text -> the text you want to send
 *    @chatID -> telegram user-ID the message will be sent to
 *    @reps -> repititions of looping through sending
 *
 *    RETURN:
 *     true -> message succesfully sent
 *     false -> message not sent, no user declared, telegram bot not set up
 */
TG_SENDING GrowingStation_smol::tg_send(String text, String* chatID, unsigned int reps) {
	if (!wifiConnected()) {
		return WIFI_ERROR;
	}
	else if (telegramBot == NULL) {
		return BOT_NOT_SETUP;
	}
	else {
		if (chatID != NULL) {
			for (int i = 0; i < reps; i++) {
				if (telegramBot->sendMessage(*chatID, text)) {
					debugPrint(text);
					debugPrint(" message successfully sent to: ");
					debugPrintln(*chatID);
					return SUCCESS;
				}
			}
		}
		else {
			int msgSent = 0;
			for (int j = 0; j < USER_MAX; j++) {
				if (users[j]->equals(userSTD)) {
					continue;
				}
				else if (tg_send(text, users[j]) != SUCCESS) {
					msgSent++;
				}
			}

			if (msgSent > 0) {
				debugPrint(text);
				debugPrint("message successfully sent to: ");
				debugPrintln(*chatID);
				return SUCCESS;
			}
			else {
				debugPrintln(" - ERROR: can't send message, invalid user! - ");
				return INVALID_USER;
			}
		}
	}
	debugPrintln(" - ERROR: can't send message, failed looping! - ");
	return LOOPING_ERROR;
}


bool GrowingStation_smol::tg_send(const char* text, String* chatID, unsigned int reps) {
	return tg_send(String(text), chatID, reps);
}



/**
 * DESCRIPTION: Read a certain amount of messages starting from the last_message_received+1
 *
 * PARAM:
 *  @chatID -> pointer to the chat-ID you want to read from (default: user1)
 *  @messageCount -> count of messages you want to read
 */
void GrowingStation_smol::tg_handleMessages(unsigned int messageCount) {
	if (telegramBot != NULL) {
		for (int i = (*telegramBot).last_message_received + 1; i <= (*telegramBot).last_message_received + messageCount; i++) {
			String readChatID = (*telegramBot).messages[i].chat_id;

			if (tg_isChatID(readChatID)) {
				// TODO: handle all the messages
			}
			else {

			}
		}
	}

}



/**
 * DESCRIPTION: Check if @chatID is in the users[]
 *
 * PARAM:
 *  @chatID -> chatID that will be checked
 *
 *  RETURN:
 *    true -> chatID is safed user in users[]
 *    false -> chatID is NOT safed in user[]
 */
bool GrowingStation_smol::tg_isChatID(String chatID) {
	for (int i = 0; i > USER_MAX; i++) {
		if (chatID.equals(*users[i])) {
			return 1;
		}
	}
	return 0;
}



/** # PRIVATE #
 * DESCRIPTION: crop the command into its individual parts recursively [command]+[param1]+[param2]+[param3]...
 *
 * PARAM:
 *	@fullCommand -> the command input by the user
 *	@lastSpace -> [ENTER NOTHING], just for the recursion
 *	@loop -> [ENTER NOTHING], just for recursion
 *
 * RETURNS:
 *	int\{-1} -> amount of params
 *	-1 -> no valid command entered
 */
int GrowingStation_smol::_cropCommand(const String fullCommand, int lastSpace, int loop) {
	if (loop < COMMAND_MAX) {
		if (loop == 0) {
			_resetCommandArray();
			fullCommand.trim();
		}
		int nextSpace = fullCommand.indexOf(" ");
		fullCommand[loop] = fullCommand.substring(lastSpace, nextSpace);
		int paramCount = _cropCommandRecursion(fullCommand, nextSpace, loop + 1);
		return paramCount;
	}
	else {
		return loop;
	}
}



/** # PRIVATE #
 * DESCRIPTION: Reset the command array to its standard values
 */
void GrowingStation_smol::_resetCommandArray() {
	for (int i = 0; i < COMMAND_MAX; i++) {
		_fullCommand[i] = _commandSTD;
	}
}





/*################################################################################
 * #---# SENSOR #---#
 *
 * various methods and functions for using an DHT-Sensor and smooth analogReading
 *################################################################################
*/


/**
 *  DESCRIPTION: read temperature with an DHT11-sensor and store it to the private variable @temperature
 *
 *  PARAM:
 *    @dht -> the dht-object you want to measure with ( setup with "dht.setup(DHT_PIN, DHTesp::DHT11)" )
 *
 *  RETURN:
 *   int\{DHT_ERROR} -> temperature rounded to int
 *   DHT_ERROR -> on sensor failure
 */
int GrowingStation_smol::dht_readTemperature(DHTesp dht) {
	_dhtDelay();
	temperature = round(dht.getTemperature());

	if (dht.getStatus() != 0) {
		return DHT_ERROR;
	}
	else {
		return temperature;
	}
}



/**
 *  DESCRIPTION: read humidity with an DHT11-sensor and store it to the private variable @temperature
 *
 *  PARAM:
 *    @dht -> the dht-object you want to measure with ( setup with "dht.setup(DHT_PIN, DHTesp::DHT11)" )
 *
 *  RETURN:
 *   int\{-1} -> temperature rounded to int
 *   DHT_ERROR -> on sensor failure
 */
int GrowingStation_smol::dht_readHumidity(DHTesp dht) {
	_dhtDelay();
	humidity = dht.getHumidity();

	if (dht.getStatus() != 0) {
		return DHT_ERROR;
	}
	else {
		return humidity;
	}
}



/**
 *  DESCRIPTION: read analog value @samples times and average it
 *
 *  PARAM:
 *    @pin -> the analog pin to be read
 *    @samples -> amount of repititions
 *
 *  RETURN:
 *   int -> smoothed analog reading
 */
int GrowingStation_smol::analogReadSmooth(unsigned int pin, unsigned int samples) {
	for (int i = 0; i < samples && i < ANALOG_MAX_SAMPLES; i++) {
		analogReads[i] = analogRead(pin);
		delay(5);
	}
	return averageArray(analogReads, samples);
}



/** # PRIVATE #
 *  DESCRIPTION: loop until the dht is allowed to measure
 */
void GrowingStation_smol::_dhtDelay() {
	while (_lastDhtRead + dht_sampling > millis()) {
		delay(50);
	}
	_lastDhtRead = millis();
}





/*################################################################################
 * #---# DEBUGGING / SERIAL #---#
 *
 * various methods and functions for the serial port checks, serial printing usw.
 *################################################################################
*/


/**
 *  DESCRIPTION: check if debugging is enabled and Serial port is available
 *
 *  RETURN:
 *   true -> Serial port is accessable
 *   false -> Serial port is unaccessable
 */
bool GrowingStation_smol::debugAvailable() {
	if (/*Serial.available() &&*/ debug) {
		return true;
	}
	else {
		return false;
	}
}



/**
 *  DESCRIPTION: Send println() message over serial port  (baudrate: 9600) when it's available and debugging is enabled
 *
 *  PARAM:
 *    @text -> debugging message to be sent over serial port
 *
 *  RETURN:
 *   true -> serial message sent
 *   false -> serial message not sent
 */
bool GrowingStation_smol::debugPrintln(const char* text) {
	if (debugAvailable()) {
		Serial.println(text);
		return true;
	}
	else {
		return false;
	}
}


bool GrowingStation_smol::debugPrintln(int number) {
	char buffer[16];
	itoa(number, buffer, 10);
	return debugPrintln(buffer);
}


bool GrowingStation_smol::debugPrintln(String text) {
	return debugPrintln(text.c_str());
}



/**
 *  DESCRIPTION: Send print() message over serial port (baudrate: 9600) when it's available and debugging is enabled
 *
 *  PARAM:
 *    @text -> debugging message to be sent over serial port
 *
 *  RETURN:
 *   true -> serial message sent
 *   false -> serial message not sent
 */
bool GrowingStation_smol::debugPrint(const char* text) {
	if (debugAvailable()) {
		Serial.print(text);
		return true;
	}
	else {
		return false;
	}
}


bool GrowingStation_smol::debugPrint(int number) {
	char buffer[16];
	itoa(number, buffer, 10);
	return debugPrint(buffer);
}


bool GrowingStation_smol::debugPrint(String text) {
	return debugPrint(text.c_str());
}





/*################################################################################
 * #---# MISCELLANEOUS #---#
 *
 * all other methods
 *################################################################################
*/


/**
 *  DESCRIPTION: convert milliseconds to  hours
 *
 *  PARAM:
 *    @millis -> milliseconds that will be calculated to hours
 *
 *  RETURN:
 *   milliseconds converted to hours
 */
float GrowingStation_smol::millisToHours(unsigned long millis) {
	return millis / 1000 / 60 / 60;
}



/**
 *  DESCRIPTION: average all the array components from 0 to @size
 *
 *  PARAM:
 *    @values -> the array to be averaged
 *    @size -> size of the array
 *
 *  RETURN:
 *   int -> averaged array values
 */
int GrowingStation_smol::averageArray(int values[], int size) {
	int sum = 0;
	for (int i = 0; i < size; i++) {
		sum += values[i];
	}
	return (sum / size);
}
