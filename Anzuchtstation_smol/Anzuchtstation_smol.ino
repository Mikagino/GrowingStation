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
	const int waterPump = 23;
	const int soilSensor = 35;
	// - DHT temp/humi sensors -
#define DHT_PIN 32
};


struct Messages {
	const String startup = "Anzuchtstation_smol rebooted...";
	const String pumped = "* Water pumped *";
};


// # measurements #
int temperature = 0;
unsigned int humidity = 0;
unsigned int soilHumidity = 0;
unsigned int lastPump_H = 0;


// # Structs #
struct GPIO gpio;
struct Messages msg;
//struct Measurements meas;
// # Objects #
DHTesp dht;
GrowingStation_smol gsm;


void setup() {
<<<<<<< HEAD
	bool setupFinished = false;
	while (!setupFinished) {
		// - setup tg-bot -
		sClient.setCACert(TELEGRAM_CERTIFICATE_ROOT);
		gsm.tg_begin(&bot, BOT_CHATID);
		gsm.wifiConnect(WIFI_SSID, WIFI_PASSWORD);

		// - setup GPIOS -
		pinMode(gpio.waterPump, OUTPUT);
		dht.setup(DHT_PIN, DHTesp::DHT11);

		// - initial analog reading (if not done, measurements won't be correct the first times) -
		//analogSetCycles(127);
		pinMode(SOIL_PIN, INPUT);
		bool attach = adcAttachPin(SOIL_PIN);
		analogSetAttenuation(ADC_11db);
		analogReadResolution(12);
		for (int i = 0; i < 3; i++) {
			gsm.soilHumidity = analogRead(SOIL_PIN);
			delay(1000);
		}

		gsm.debugPrintln("\n # Bot initialized # ");
		gsm.debugPrintln(attach ? "ADC-pin attached" : "ADC-pin not attached");
		setupFinished = gsm.tg_send("Anzuchtstation_smol rebooted...");
		if (setupFinished) {
			gsm.debugPrintln("# Setup finished #");
		}
		else {
			gsm.debugPrintln("# ERROR: Setup restarting! #");
		}

#ifdef MANUAL_WATERING
		pinMode(MANUAL_WATERING_INPUT, INPUT);
#endif
=======
	pinMode(gpio.waterPump, OUTPUT);
	dht.setup(DHT_PIN, DHTesp::DHT11);
	gsm.tg_begin(BOT_TOKEN, BOT_CHATID);
	gsm.wifiConnect(WIFI_SSID, WIFI_PASSWORD);

	// - initial analog reading (if not done, measurements won't be correct the first times) -
	analogSetCycles(127);
	for (int i = 0; i < 3; i++) {
		gsm.soilHumidity = analogRead(gpio.soilSensor);
		delay(3000);
>>>>>>> 2de77f86bc7f3e33010b8a7237a168d6b8294093
	}
}



void loop() {
	// - measure temperature, humidity and soil-humidity until the dht returns no error -
	int errorCount = 0;
	do {
		gsm.dht_readTemperature(dht);
		gsm.dht_readHumidity(dht);
		gsm.soilHumidity = analogRead(SOIL_SENSOR_INPUT1);
		delay(100);

		if (gsm.temperature == DHT_ERROR || gsm.humidity == DHT_ERROR) {
			errorCount++;
			gsm.debugPrint("\nError reading DHT values - ErrorCount: "); gsm.debugPrintln(errorCount);
		}

	} while (gsm.temperature == DHT_ERROR || gsm.humidity == DHT_ERROR);

	// - print out all the measurements -
	/*gsm.debugPrint("Air Temp: "); gsm.debugPrint(gsm.temperature); gsm.debugPrintln("°C");
	gsm.debugPrint("Air Humi: "); gsm.debugPrint(gsm.humidity); gsm.debugPrintln("%");*/
	gsm.debugPrint("Soil Humi: "); gsm.debugPrint(gsm.soilHumidity); gsm.debugPrintln(" (mV)");

	// - pump water when @soilHumidity is over @watering_treshold and only all @watering_frequencyH hours -
	unsigned int currentTime_H = gsm.millisToHours(millis());
	unsigned int pumpTimeTreshold_H = lastPump_H + gsm.watering_frequency_H;

	if (currentTime_H >= pumpTimeTreshold_H && gsm.soilHumidity > gsm.watering_treshold) {
		gsm.pumpWater();
	}
#ifdef MANUAL_WATERING
	else if (digitalRead(MANUAL_WATERING_INPUT) == HIGH) {
		gsm.pumpWater();
	}
#endif

	gsm.tg_handleMessages();
	delay(gsm.loop_delay);
}
