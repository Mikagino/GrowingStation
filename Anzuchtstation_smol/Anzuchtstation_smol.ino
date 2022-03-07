#include "GrowingStation_smol.h"

#define DEBUG
#ifdef DEBUG
#define debugln(x) Serial.println(x)
#define debug(x) Serial.print(x)
#else
#define debugln(x)
#define debug(x)
#endif


// # Input-/Output-Pins in struct for better overview #
struct GPIO {
  const unsigned int waterPump = 23;
  const unsigned int soilSensor = 2;
  // - DHT temp/humi sensors -
#define DHT_PIN 32
};


struct Messages{
  const String start = "Anzuchtstation rebooted...";
  const String pumped = "Water pumped!";
};


// # measurements #
int temperature = 0;
unsigned int humidity = 0;
unsigned int soilHumidity = 0;
unsigned long lastPump = 0;


// # Structs #
struct GPIO gpio;
struct Messages msg;
//struct Measurements meas;
// # Objects #
DHTesp dht;
GrowingStation_smol gsm;


void setup() {
  pinMode(gpio.waterPump, OUTPUT);
  dht.setup(DHT_PIN, DHTesp::DHT11);
  Serial.begin(9600);
  debugln(" # Bot initialized # ");
}


void loop() {
  debugln(" - loop - ");

  int errorCount = 0;
  do{
      debugln(" * measure * ");
  
    gsm.temperature = gsm.DHT_ReadTemperature(dht);
    delay(gsm.dht_sampling);
    gsm.humidity = gsm.DHT_ReadHumidity(dht);
    delay(gsm.dht_sampling);
    gsm.soilHumidity = analogRead(gpio.soilSensor);
    if(gsm.temperature == DHT_ERROR || gsm.humidity == DHT_ERROR){
      errorCount++;
      debug("\nError reading DHT values - ErrorCount: "); debugln(errorCount);
    }
  } while(gsm.temperature == DHT_ERROR  || gsm.humidity == DHT_ERROR);
  
  debug("Air Temp: "); debug(gsm.temperature); debugln("°C");
  debug("Air Humi: "); debug(gsm.humidity); debugln("%");
  debug("Soil Humi: "); debug(gsm.soilHumidity); debugln(" (no unit yet)");

  int pumpTimeTreshold = gsm.millisToHours(millis()) - lastPump - gsm.watering_frequencyH;
  
  if(pumpTimeTreshold >= 0 && gsm.soilHumidity < 3000){
    lastPump = gsm.millisToHours(millis());
    digitalWrite(gpio.waterPump, 1);
    debugln(" * Water pumped * ");
    delay(gsm.watering_delay_amount);
    digitalWrite(gpio.waterPump, 0);
  }
  
  delay(gsm.loop_delay);
}
