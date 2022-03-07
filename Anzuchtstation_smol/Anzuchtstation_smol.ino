#include "GrowingStation_smol.h"

#define DEBUG
#ifdef DEBUG
#define debugln(x) Serial.println(x)
#define debug(x) Serial.print(x)
#else
#define debugln(x)
#define debug(x)
#endif

#define DHT_SAMPLING 3000
#define LOOP_DELAY 10000
#define PUMP_HOURS_DELAY 0
#define PUMP_AMOUNT_DELAY 5000

// # Input-/Output-Pins in struct for better overview #
struct GPIO {
  const unsigned int waterPump = 23;
  const unsigned int soilSensor = 2;
  // - DHT temp/humi sensors -
#define DHT_PIN 12
};


struct Messages{
  const String start = "Anzuchtstation rebooted...";
  const String pumped = "Water pumped!";
};


struct Measurements{
  int temperature;
  int humidity;
  int soilHumidity;

  unsigned long lastPump;
  
  Measurements(){
    temperature = 0;
    humidity = 0;
    soilHumidity = 0;
    lastPump = PUMP_HOURS_DELAY;
  }
};


// - Structs -
struct GPIO gpio;
struct Messages msg;
struct Measurements meas;
// - Objects -
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
  
  meas.temperature = 0;
  meas.humidity = 0;
  meas.soilHumidity = 0;

  int errorCount = 0;
  do{
      debugln(" * measure * ");
  
    meas.temperature = gsm.DHT_ReadTemperature(dht);
    delay(DHT_SAMPLING);
    meas.humidity = gsm.DHT_ReadHumidity(dht);
    delay(DHT_SAMPLING);
    meas.soilHumidity = analogRead(gpio.soilSensor);
    if(meas.temperature == DHT_ERROR || meas.humidity == DHT_ERROR){
      errorCount++;
      debug("\nError reading DHT values - ErrorCount: "); debugln(errorCount);
    }
  } while(meas.temperature == DHT_ERROR  || meas.humidity == DHT_ERROR);
  
  debug("Air Temp: "); debug(meas.temperature); debugln("°C");
  debug("Air Humi: "); debug(meas.humidity); debugln("%");
  debug("Soil Humi: "); debug(meas.soilHumidity); debugln(" (no unit yet)");

  int pumpTimeTreshold = gsm.millisToHours(millis()) - meas.lastPump - PUMP_HOURS_DELAY;
  
  if(pumpTimeTreshold >= 0 && meas.soilHumidity < 3000){
    meas.lastPump = gsm.millisToHours(millis());
    digitalWrite(gpio.waterPump, 1);
    debugln(" * Water pumped * ");
    delay(PUMP_AMOUNT_DELAY);
    digitalWrite(gpio.waterPump, 0);
  }
  
  delay(LOOP_DELAY);
}
