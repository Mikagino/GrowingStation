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


// #---# Bot commands #---#
// #- Comment following line if you want to use G-Codes -#
//#define LONG_CMDS
#ifdef LONG_CMDS
#define CMD_HELP "/help"  // Send list of commands to telegram user
// _ Ask for informations _
#define CMD_GET-MEASUREMENTS "/sendMeasurements"  //send air-temperature, air-humidity and soil-humidity
// _ User informations _
#define CMD_USER-ADD "/addUser" //add a telegram-ChatID "/addUser >String chatID<"
#define CMD_USER-RETURN "/returnUser" //return all the telegram-ChatIDs
// _ setting values _
#define CMD_SET-DHTDELAY "/setDHTsampling"
#define CMD_SET-LOOPDELAY "/setLoopDelay"
#define CMD_SET-PUMPDELAY "/setPumpDelay"
#define CMD_SET-HUMITRESH "/setHumiTresh"
#define CMD_SET-SENDDELAY "/setSendDelay"
#define CMD_SET-DEBUG "/setDebug"

#else
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
#endif


class GrowingStation_smol{
  private:
    // +--+ dht +--+
    unsigned long _lastDhtRead = 0;
    #define DHT11_SAMPLING 1000;
    #define DHT22_SAMPLING 2000;

  // #---# DHT-SENSOR #---#
  // #- various methods and functions for using the DHT-sensor -#
    void _dhtDelay();    


  
  public:
  // +--+ system +--+
  bool debug = true;

  
  // +--+ delays (most of them in milliseconds) +--+
  unsigned long dht_sampling = 1000; // - delay between dht reading -
  unsigned long loop_delay = 5000; // - delay between entire loops -
  unsigned int watering_frequencyH = 24; // - how often it's watered (in hours) -
  unsigned long watering_amount = 5000; // - how long the pump is activated for watering
  unsigned long watering_treshold = 3300; // - on what soilHumidity the pump will be activated -


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

  
  // - measurements -
  int temperature = 0;
  unsigned int humidity = 0;
  unsigned int soilHumidity = 0;

  
  // #---# TELEGRAM BOT #---#
  // #- various methods and functions for using the telegram bot -#
  void begin(const String newBotToken, String chatID);
  bool sendingLoop(String text, String *chatID, unsigned int reps = 5);
  void handleMessages(unsigned int messageCount = 1);
  bool isChatID(String chatID);

  
  // #---# DHT-SENSOR #---#
  // #- various methods and functions for using the DHT-sensor -#
  int dht_readTemperature(DHTesp dht);
  int dht_readHumidity(DHTesp dht);


  // #---# DEBUGGING/SERIAL #---#
  // #- various methods and functions about serial port checks, serial printing usw. -#
  bool debugAvailable();
  bool debugPrintln(const char* text);
  bool debugPrint(const char* text);
  bool debugPrintln(int number);
  bool debugPrint(int number);

  
  // #---# MISCELLANEOUS #---#
  // #- various methods and functions about serial port checks, serial printing usw. -#
  float millisToHours(unsigned long millis);
};
