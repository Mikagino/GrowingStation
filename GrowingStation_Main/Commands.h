// #---# Bot commands #---#
// #- Comment following line if you want to use G-Codes -#
#define CMD_COMMAND 0
#define CMD_PARAM_1 1
#define CMD_PARAM_2 2
#define CMD_PARAM_3 3
//#define LONG_CMDS
#ifdef LONG_CMDS
#define COMMAND_LENGTH 20
// _ general system infos _
#define CMD_HELP "/help" // Send list of commands to telegram user
// _ get station information _
#define CMD_GET_TEMP "/temp" // get air-temperature
#define CMD_GET_HUMI "humi" // get air-humidity
// _ telegram informations _
#define CMD_USERADD "/addUser"  //add a telegram-ChatID "G201 >String chatID<"
#define CMD_GETUSERS "/users" //return all the telegram-ChatIDs
// _ set values to command followed by number "/... + xxxxx" _
#define CMD_SETDHTSAMPLING "/setDHT"
#define CMD_SETLOOPDELAY "/setLoopD"
#define CMD_SETWATERINGAMOUNT "/setWaterAmount"
#define CMD_WATERINGFREQUENCY "/setWaterFreq"
#define CMD_SETHUMITRESH "/setHumiTresh"
#define CMD_SETDEBUG "/setDebug"
// _ EASTER EGG _
#define KEK "KEK"

#else
#define COMMAND_LENGTH 4
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