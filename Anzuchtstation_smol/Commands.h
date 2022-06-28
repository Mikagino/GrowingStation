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
#define SYSTEM_INFOS '0'
#define CMD_HELP "G000" // Send list of commands to telegram user
// _ get station information "G1xx" _
#define GETTER '1'
#define CMD_GET_TEMP "G100" // get air-temperature
#define CMD_GET_HUMI "G101" // get air-humidity
// _ telegram informations "G2xx" _
#define TELEGRAM_INFOS '2'
#define CMD_USERADD "G201"  //add a telegram-ChatID "G201 >String chatID<"
#define CMD_GETUSERS "G202" //return all the telegram-ChatIDs
// _ set values to command followed by number "G5xx + xxxxx" _
#define SETTER '3'
#define CMD_SETDHTSAMPLING "G300"
#define CMD_SETLOOPDELAY "G301"
#define CMD_SETWATERINGAMOUNT "G302"
#define CMD_WATERINGFREQUENCY "G303"
#define CMD_SETHUMITRESH "G304"
#define CMD_SETDEBUG "G305"
// _ EASTER EGG _
#define KEK "KEK"
#endif