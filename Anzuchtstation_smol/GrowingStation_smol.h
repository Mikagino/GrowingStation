#include "DHTesp.h"
#include <UniversalTelegramBot.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#define DHT_ERROR -173


class GrowingStation_smol{
  private:
  
  public:
  // - delays (most of them in milliseconds) -
  unsigned long dht_sampling = 1000;
  unsigned long loop_delay = 5000;
  unsigned int watering_frequencyH = 0; // - how often it's watered (in hours) -
  unsigned long watering_delay_amount = 5000;

  // - telegram bot -
  UniversalTelegramBot* telegramBot = NULL;

  // - users -
  String userSTD = "XXXXXXXXX";
  String user1;
  String user2;
  String user3;
  String user4;
  String user5;
  #define USER_MAX
  String* users[USER_MAX];
  unsigned int userCount = 0;
  
  // - measurements -
  int temperature = 0;
  unsigned int humidity = 0;
  unsigned int soilHumidity = 0;

  void begin(const String newBotToken, String chatID);
  int DHT_ReadTemperature(DHTesp dht);
  int DHT_ReadHumidity(DHTesp dht);
  int millisToHours(unsigned long millis);
};
