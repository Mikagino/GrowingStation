#include "GrowingStation_smol.h"



/** DESCRIPTION: setup the telegram bot, serial output on Baudrate 9600, assign user pointer array to variables
 *  
 *  PARAM:
 *  
 *  RETURN:
 */
void GrowingStation_smol::tg_begin(const String newBotToken, String chatID) {
  WiFiClientSecure sClient;
  UniversalTelegramBot bot = UniversalTelegramBot(newBotToken, sClient);
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
}


void GrowingStation_smol::tg_read() {

}



/** DESCRIPTION: read temperature with an DHT11-sensor and store it to the private variable @temperature
 *  
 *  PARAM: 
 *    @dht -> the dht-object you want to measure with ( setup with "dht.setup(DHT_PIN, DHTesp::DHT11)" )
 *  
 *  RETURN:
 *   int\{DHT_ERROR} -> temperature rounded to int
 *   DHT_ERROR -> on sensor failure
 */
int GrowingStation_smol::dht_readTemperature(DHTesp dht) {
  temperature = round(dht.getTemperature());

  if (dht.getStatus() != 0) {
    return DHT_ERROR;
  }
  else {
    return temperature;
  }
}



/** DESCRIPTION: read humidity with an DHT11-sensor and store it to the private variable @temperature
 *  
 *  PARAM: 
 *    @dht -> the dht-object you want to measure with ( setup with "dht.setup(DHT_PIN, DHTesp::DHT11)" )
 *  
 *  RETURN:
 *   int\{-1} -> temperature rounded to int
 *   DHT_ERROR -> on sensor failure
 */
int GrowingStation_smol::dht_readHumidity(DHTesp dht) {
  humidity = dht.getHumidity();

  if (dht.getStatus() != 0) {
    return DHT_ERROR;
  }
  else {
    return humidity;
  }
}



/** DESCRIPTION: convert milliseconds to rounded hours
 *  
 *  PARAM: 
 *    @millis -> milliseconds that will be calculated to hours
 *  
 *  RETURN:
 *   milliseconds converted to hours
 */
int GrowingStation_smol::millisToHours(unsigned long millis){
  return round(millis/1000/60/60);
}
