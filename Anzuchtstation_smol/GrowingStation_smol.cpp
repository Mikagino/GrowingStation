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
void GrowingStation_smol::wifiConnect(const char *ssid, const char *pswd)
{
	WiFi.begin(ssid, pswd);
	while (!wifiConnected())
	{
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
bool GrowingStation_smol::wifiConnected()
{
	if (WiFi.status() != WL_CONNECTED)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/* ################################################################################
 * #---# TELEGRAM BOT #---#
 *
 * various methods and functions for using the telegram bot with an ESP32 or ESP2866
 * ################################################################################
 */

void GrowingStation_smol::pumpWater(int pumpIndex)
{
	if (pumpIndex == ALL_PUMPS)
	{
		gsm.debugPrintln(msg.pumped);
		gsm.tg_send(msg.pumped);
		lastPump_H = gsm.millisToHours(millis());
		digitalWrite(gpio.waterPump, 1);
		delay(gsm.watering_amount);
		digitalWrite(gpio.waterPump, 0);
	}
}

/* ################################################################################
 * #---# TELEGRAM BOT #---#
 *
 * various methods and functions for using the telegram bot
 * ################################################################################
 */

/*
 *  DESCRIPTION: setup serial output on Baudrate 9600, assign user/command pointer array to variables
 *
 *  PARAM:
 *    @botToken -> the token of the telegram bot (got from botfather on telegram, when creating bot)
 *    @chatID -> first user-id (got from ID-bot on telegram)
 */
void GrowingStation_smol::tg_begin(UniversalTelegramBot *bot, String chatID)
{
	telegramBot = bot;

	users[0] = &user1;
	users[1] = &user2;
	users[2] = &user3;
	users[3] = &user4;
	users[4] = &user5;

	usersWrite[0] = &user1_Write;
	usersWrite[1] = &user2_Write;
	usersWrite[2] = &user3_Write;
	usersWrite[3] = &user4_Write;
	usersWrite[4] = &user5_Write;

	*users[0] = chatID;
	userCount++;

	_fullCommand[0] = &_command;
	_fullCommand[1] = &_param1;
	_fullCommand[2] = &_param2;
	_fullCommand[3] = &_param3;

	//_setupCommands();

	Serial.begin(9600);
}



/**
 *  DESCRIPTION: Try connecting to the wifi until it works
 *
 *  PARAM:
 *    @ssid -> name of the wifi
 *	  @pswd -> password of the wifi
 */
void GrowingStation_smol::wifiConnect(const char *ssid, const char *pswd)
{
	WiFi.begin(ssid, pswd);
	while (WiFi.status() != WL_CONNECTED)
	{
		debugPrintln("WiFi try connecting...");
		delay(1000);
	}
	debugPrintln("WiFi connected!");
}



/**
 *  DESCRIPTION: Try sending the message @reps times with the universal telegram bot to an specific @chatID or to all, when @chatID is NULL
 *
 *  PARAM:
 *    @text -> the text you want to send
 *    @chatID -> telegram user-ID the message will be sent to
 *    @reps -> repititions of looping through sending
 *
 *    RETURN:
 *		 1 = SUCCESS
 *		-1 = SENDING_ERROR
 *		-2 = INVALID_USER
 *		-3 = BOT_NOT_SETUP
 */
TG_SENDING GrowingStation_smol::tg_send(String text, String *chatID, unsigned int reps)
{
	if (!wifiConnected())
	{
		return WIFI_ERROR;
	}
	else if (telegramBot == NULL)
	{
		return BOT_NOT_SETUP;
	}
	else {
		if (chatID != NULL) {
			for (int i = 0; i < reps; i++) {
				if (chatID->equals(userSTD)) {
					continue;
				}
				else if (telegramBot->sendMessage(*chatID, text)) {
					debugPrint("'"); debugPrint(text); debugPrint("'"); debugPrint(" successfully sent to: "); debugPrintln(*chatID);
					return SUCCESS;
				}
			}
		}
		else
		{
			int msgSent = 0;
			for (int j = 0; j < USER_MAX; j++) {
				if (*users[j] == NULL || users[j]->equals(userSTD)) {
					continue;
				}
				else if (tg_send(text, users[j]) == SUCCESS)
				{
					msgSent++;
				}
			}

			if (msgSent > 0)
			{
				debugPrint("==> '");
				debugPrint(text);
				debugPrint("'");
				debugPrint(" successfully sent to: ");
				debugPrint(msgSent);
				debugPrintln(" users");
				return SUCCESS;
			}
			else
			{
				debugPrintln(" - ERROR: can't send message, invalid user! - ");
				return INVALID_USER;
			}
		}
	}
	debugPrintln(" - ERROR: can't send message, failed looping! - ");
	return LOOPING_ERROR;
}

bool GrowingStation_smol::tg_send(const char *text, String *chatID, unsigned int reps)
{
	return tg_send(String(text), chatID, reps);
}

/**
 * DESCRIPTION: Read in chat information and respond (commands described in "Commands.h")
 *
 * PARAM:
 *  @chatID -> pointer to the chat-ID you want to read from (default: user1)
 *  @messageCount -> count of messages you want to read
 */
void GrowingStation_smol::tg_handleMessages()
{
	int msgNum = telegramBot->getUpdates(telegramBot->last_message_received + 1);
	if (telegramBot != NULL)
	{
		for (int i = 0; i < msgNum; i++)
		{
			String currentChatID = telegramBot->messages[i].chat_id;
			String currentChatName = telegramBot->messages[i].from_name;
			String currentChatText = telegramBot->messages[i].text;
			bool currentChatPermission = *usersWrite[tg_getChatID(currentChatID)];

			String debugCurrentMsg = "Debug: ";
			debugCurrentMsg += currentChatText;
			debugCurrentMsg += " entered by ";
			debugCurrentMsg += currentChatName;
			debugPrintln(debugCurrentMsg);

			if (tg_isChatID(currentChatID))
			{
				String text;

				// - System info commands -
				if (currentChatText.charAt(1) == SYSTEM_INFOS)
				{
					if (currentChatText.equals(CMD_HELP))
					{
						text = String("## COMMANDS ##");
						text += String("\n\nSystem Informations");
						text += String("\n  Help: ") + CMD_HELP;
						text += String("\n\nGetters");
						text += String("\n  Temperature: ") + CMD_GET_TEMP;
						text += String("\n  Humidity: ") + CMD_GET_HUMI;
						text += String("\n\nSetters");
						text += String("\n  DHT-Sampling: ") + CMD_SETDHTSAMPLING + " [0000]ms";
						text += String("\n  Loop-Delay: ") + CMD_SETLOOPDELAY + " [0000]ms";
						text += String("\n  Watering-Amount: ") + CMD_SETWATERINGAMOUNT + " [0000]ms";
						text += String("\n  Watering-Frequency: ") + CMD_WATERINGFREQUENCY + " [0000]h";
						text += String("\n  Humidity-Treshold: ") + CMD_SETHUMITRESH + " [0000]mV";
						text += String("\n  Debugging: ") + CMD_SETDEBUG + " [1:true/0:false]";
					}
					tg_send(text, &currentChatID);
				}

				// - Getter commands -
				else if (currentChatText.charAt(1) == GETTER)
				{
					if (currentChatText.equals(CMD_GET_TEMP))
					{
						text = String("Temperature: ") + temperature + "C";
					}
					else if (currentChatText.equals(CMD_GET_HUMI))
					{
						text = String("Humidity: ") + humidity + "%";
					}
					tg_send(text, &currentChatID);
				}
				// - Setter commands -
				else if (currentChatText.charAt(1) == SETTER)
				{

					// TODO: FIND ERROR in syntax!!! XD

					if (!currentChatPermission)
					{
						text = String("You don't have the permission to change variables!");
						text += String("\nAsk an admin to change your writing permission\n");
						text += String("Use ") + CMD_GETUSERS + " to get all users(with permission info)";
						tg_send(text, &currentChatID);
					}
					else
					{
						_cutCommand(currentChatText);
						// # DHT-sampling #
						if (currentChatText.equals(CMD_SETDHTSAMPLING))
						{
							int value = _param1.toInt();
							if (value != 0)
							{
								dht_sampling = value;
								text = String("@dht_sampling set to ") + dht_sampling;
							}
							else
							{
								text = String("ERROR: ") + _param1 + " is an invalid parameter!";
							}
						}
						// # Loop-Delay #
						else if (currentChatText.equals(CMD_SETLOOPDELAY))
						{
							int value = _param1.toInt();
							if (value != 0)
							{
								loop_delay = value;
								text = String("@loop_delay set to ") + loop_delay;
							}
							else
							{
								text = String("ERROR: ") + _param1 + " is an invalid parameter!";
							}
						}
						// # Debugging #
						else if (currentChatText.equals(CMD_SETDEBUG))
						{
							if (isBool(_param1))
							{
								debug = toBool(_param1);
								text = String("@Debug set to ") + _param1;
							}
							else
							{
								text = String("ERROR: ") + _param1 + " is an invalid parameter!";
							}
						}
						tg_send(text, &currentChatID);
					}
				}
				else if (currentChatText.equals(KEK))
				{
					text = String("You're a kek yourself! ;^;");
					tg_send(text, &currentChatID);
				}
				else
				{
					text += String("ERROR: ") + currentChatText + " is an invalid command!";
					tg_send(text, &currentChatID);
				}
			}
			// - Command error -
			else
			{
				String text = String("ERROR: ") + "You're an invalid user, " + currentChatName + "!";
				tg_send(text, &currentChatID);
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
bool GrowingStation_smol::tg_isChatID(String chatID)
{
	for (int i = 0; i < USER_MAX; i++)
	{
		if (chatID.equals(*users[i]))
		{
			return 1;
		}
	}
	return 0;
}

/**
 * DESCRIPTION: Check if @chatID is in users[]
 *
 * PARAM:
 *  @chatID -> chatID that will be checked
 *
 *  RETURN:
 *		is user -> Array index of users (read and write rights are saved in userWrite[x])
 */
int GrowingStation_smol::tg_getChatID(String chatID)
{
	for (int i = 0; i < USER_MAX; i++)
	{
		if (chatID.equals(*users[i]))
		{
			return i;
		}
	}
	return -1;
}

/** # PRIVATE #
 * DESCRIPTION: setup all the commands for the bot
 */
// void GrowingStation_smol::_setupCommands() {
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
// }

/*################################################################################
 * #---# Commands #---#
 *
 * various methods and functions for the telegram commands
 *################################################################################
 */

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
int GrowingStation_smol::_cropCommand(const String fullCommand, int lastSpace, int loop)
{
	if (loop < COMMAND_MAX)
	{
		if (loop == 0)
		{
			_resetCommandArray();
			fullCommand.trim();
		}
		int nextSpace = fullCommand.indexOf(" ");
		fullCommand[loop] = fullCommand.substring(lastSpace, nextSpace);
		int paramCount = _cropCommandRecursion(fullCommand, nextSpace, loop + 1);
		return paramCount;
	}
	else
	{
		return loop;
	}
}

/** # PRIVATE #
 * DESCRIPTION: Reset the command array to its standard values
 */
void GrowingStation_smol::_resetCommand() {

}

void GrowingStation_smol::_resetCommandArray()
{
	for (int i = 0; i < COMMAND_MAX; i++)
	{
		_fullCommand[i] = _commandSTD;
	}
}

/** # PRIVATE #
 * DESCRIPTION: Stores the command and the params into the _fullCommand String array
 *
 * RETURN:
 *	PARAM_ERROR -> too many params
 *	FUNCTION_STOP -> function stopped neutral
 */
CMD_STATUS GrowingStation_smol::_cutCommand(String cmd, unsigned int loop)
{
	int cmdLength = cmd.length();
	if (loop > 3)
	{
		return PARAM_ERROR;
	}
	if (cmdLength != 0)
	{
		for (unsigned int i = 0; i < cmdLength; i++)
		{
			if (cmd.charAt(i) == ' ')
			{
				*_fullCommand[loop] = cmd.substring(0, i);
				_cutCommand(cmd.substring(i, cmdLength), loop++);
			}
		}
		return loop;
	}
	return FUNCTION_STOP;
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
int GrowingStation_smol::dht_readTemperature(DHTesp dht)
{
	_dhtDelay();
	temperature = round(dht.getTemperature());

	if (dht.getStatus() != 0)
	{
		return DHT_ERROR;
	}
	else
	{
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
int GrowingStation_smol::dht_readHumidity(DHTesp dht)
{
	_dhtDelay();
	humidity = dht.getHumidity();

	if (dht.getStatus() != 0)
	{
		return DHT_ERROR;
	}
	else
	{
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
int GrowingStation_smol::analogReadSmooth(unsigned int pin, unsigned int samples)
{
	for (int i = 0; i < samples && i < ANALOG_MAX_SAMPLES; i++)
	{
		analogReads[i] = analogRead(pin);
		delay(5);
	}
	return averageArray(analogReads, samples);
}

/** # PRIVATE #
 *  DESCRIPTION: loop until the dht is allowed to measure
 */
void GrowingStation_smol::_dhtDelay()
{
	while (_lastDhtRead + dht_sampling > millis())
	{
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
bool GrowingStation_smol::debugAvailable()
{
	if (/*Serial.available() &&*/ debug)
	{
		return true;
	}
	else
	{
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
bool GrowingStation_smol::debugPrintln(const char *text)
{
	if (debugAvailable())
	{
		Serial.println(text);
		return true;
	}
	else
	{
		return false;
	}
}

bool GrowingStation_smol::debugPrintln(int number)
{
	char buffer[16];
	itoa(number, buffer, 10);
	return debugPrintln(buffer);
}

bool GrowingStation_smol::debugPrintln(String text)
{
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
bool GrowingStation_smol::debugPrint(const char *text)
{
	if (debugAvailable())
	{
		Serial.print(text);
		return true;
	}
	else
	{
		return false;
	}
}

bool GrowingStation_smol::debugPrint(int number)
{
	char buffer[16];
	itoa(number, buffer, 10);
	return debugPrint(buffer);
}

bool GrowingStation_smol::debugPrint(String text)
{
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
float GrowingStation_smol::millisToHours(unsigned long millis)
{
	return millis / 1000 / 60 / 60;
}



/*
 * DESCRIPTION: Check if string is a boolean
 */
bool GrowingStation_smol::isBool(String string)
{
	if (string.equals("true") || string.equals("1") || string.equals("false") || string.equals("0"))
	{
		return true;
	}
	return false;
}

/*
 * DESCRIPTION: Return boolean value of String
 */
bool GrowingStation_smol::toBool(String string)
{
	if (string.equals("true") || string.equals("1"))
	{
		return true;
	}
	return false;
}
