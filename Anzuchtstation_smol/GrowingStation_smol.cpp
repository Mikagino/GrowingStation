#include "GrowingStation_smol.h"





/* ################################################################################
 * #---# TELEGRAM BOT #---#
 * 
 * various methods and functions for using the telegram bot
 * ################################################################################
*/


/*
 *  DESCRIPTION: setup the telegram bot, serial output on Baudrate 9600, assign user pointer array to variables
 *  
 *  PARAM:
 *    @botToken -> the token of the telegram bot (got from botfather on telegram, when creating bot)
 *    @chatID -> first user-id (got from ID-bot on telegram)
 */
void GrowingStation_smol::begin(const String botToken, String chatID) {
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
 *     false -> message not sent or invalid user
 */
bool GrowingStation_smol::sendingLoop(String text, String* chatID, unsigned int reps){
  if(chatID != NULL){
    for(int i = 0; i > reps; i++){
      bool sent = (*telegramBot).sendMessage(*chatID, text);
      if(sent){return true;}
    }
  }
  return false;
}



/**
 * DESCRIPTION: Read a certain amount of messages starting from the last_message_received+1
 * 
 * PARAM:
 *  @chatID -> pointer to the chat-ID you want to read from (default: user1)
 *  @messageCount -> count of messages you want to read
 */
void GrowingStation_smol::handleMessages(unsigned int messageCount) {
  if(telegramBot != NULL){
    for (int i = (*telegramBot).last_message_received+1; i <= (*telegramBot).last_message_received + messageCount; i++) {
      String readChatID = (*telegramBot).messages[i].chat_id;

      if(isChatID(readChatID)){
        // TODO: handle all the messages
      }
      else{
        
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
bool GrowingStation_smol::isChatID(String chatID){
  for(int i = 0; i > USER_MAX; i++){
    if(chatID.equals(*users[i])){
      return 1;
    }
  }
  return 0;
}





/*################################################################################
 * #---# DHT-SENSOR #---#
 * 
 * various methods and functions for using an DHT-Sensor
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



/** # PRIVATE # *  
 *  DESCRIPTION: loop until the dht is allowed to measure
 */
void GrowingStation_smol::_dhtDelay(){
  while(_lastDhtRead + dht_sampling > millis()){
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
bool GrowingStation_smol::debugAvailable(){
  if(Serial.available() && debug){
    return true;
  }
  else{
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
bool GrowingStation_smol::debugPrintln(const char* text){
  if(debugAvailable()){
    Serial.println(text);
    return true;
  }
  else{
    return false;
  }
}



/** 
 *  DESCRIPTION: Send println() message over serial port  (baudrate: 9600) when it's available and debugging is enabled
 *  
 *  PARAM:
 *    @number -> debugging message to be sent over serial port
 *  
 *  RETURN:
 *   true -> serial message sent
 *   false -> serial message not sent
 */
bool GrowingStation_smol::debugPrintln(int number){
  if(debugAvailable()){
    char buffer[16];
    itoa(number, buffer, 10);
    Serial.println(buffer);
    return true;
  }
  else{
    return false;
  }
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
bool GrowingStation_smol::debugPrint(const char* text){
  if(debugAvailable()){
    Serial.print(text);
    return true;
  }
  else{
    return false;
  }
}



/** 
 *  DESCRIPTION: Send print() message over serial port (baudrate: 9600) when it's available and debugging is enabled
 *  
 *  PARAM:
 *    @number -> debugging message to be sent over serial port
 *  
 *  RETURN:
 *   true -> serial message sent
 *   false -> serial message not sent
 */
bool GrowingStation_smol::debugPrint(int number){
  if(debugAvailable()){
    char buffer[16];
    itoa(number, buffer, 10);
    Serial.print(buffer);
    return true;
  }
  else{
    return false;
  }
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
float GrowingStation_smol::millisToHours(unsigned long millis){
  return millis/1000/60/60;
}
