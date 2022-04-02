#include "GrowingStation_smol.h"

/*#define DEBUG
#ifdef DEBUG
#define debugln(x) Serial.println(x)
#define debug(x) Serial.print(x)
#else
#define debugln(x)
#define debug(x)
#endif*/


// # Input-/Output-Pins in struct for better overview #
struct GPIO {
	const unsigned int waterPump = 23;
	const unsigned int soilSensor = 2;
	// - DHT temp/humi sensors -
#define DHT_PIN 32
};


struct Messages {
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
	gsm.debugPrintln(" # Bot initialized # ");
	gsm.begin(BOT_TOKEN, BOT_CHATID);

  // - initial serial reading (if not done, measurements won't be correct the first times) -
  for(int i = 0; i < 3; i++){
    gsm.soilHumidity = analogRead(gpio.soilSensor);
    delay(3000);
  }
}



void loop() {
	gsm.debugPrintln(" - loop - ");

  // - measure temperature, humidity and soil-humidity until the dht returns no error -
	int errorCount = 0;
	do {
		gsm.debugPrintln(" * measure * ");
		gsm.dht_readTemperature(dht);
		gsm.dht_readHumidity(dht);
		gsm.soilHumidity = analogRead(gpio.soilSensor);
   
		if (gsm.temperature == DHT_ERROR || gsm.humidity == DHT_ERROR) {
			errorCount++;
			gsm.debugPrint("\nError reading DHT values - ErrorCount: "); gsm.debugPrintln(errorCount);
		}
   
	} while (gsm.temperature == DHT_ERROR || gsm.humidity == DHT_ERROR);

  // - print out all the measurements -
	gsm.debugPrint("Air Temp: "); gsm.debugPrint(gsm.temperature); gsm.debugPrintln("°C");
	gsm.debugPrint("Air Humi: "); gsm.debugPrint(gsm.humidity); gsm.debugPrintln("%");
	gsm.debugPrint("Soil Humi: "); gsm.debugPrint(gsm.soilHumidity); gsm.debugPrintln(" (no unit yet)");

  // - pump water when @soilHumidity is over @watering_treshold and only all @watering_frequencyH hours -
	int pumpTimeTreshold = gsm.millisToHours(millis()) - lastPump - gsm.watering_frequencyH;

	if (pumpTimeTreshold >= 0 && gsm.soilHumidity > gsm.watering_treshold) {
		lastPump = gsm.millisToHours(millis());
		digitalWrite(gpio.waterPump, 1);
		gsm.debugPrintln(" * Water pumped * ");
		delay(gsm.watering_amount);
		digitalWrite(gpio.waterPump, 0);
	}

	delay(gsm.loop_delay);
}
