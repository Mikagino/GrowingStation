#define _DISABLE_TLS_
#include "GrowingStation_smol.h"

#ifdef TELEGRAM_BOT
// # Bot objects #
WiFiClientSecure sClient;
UniversalTelegramBot bot(BOT_TOKEN, sClient);
char BOT_CHATID[USER_LENGTH] = "594278076";
#endif // TELEGRAM_BOT


// # Station objects #
DHTesp dht;
GrowingStation_smol gsm;
MP_Delay mpd;
//MP_Delay mpd_dht;



/*################################################################################
*
 * #---# INTERRUPTS #---#
 *
 *################################################################################
*/


volatile bool wateringMode = AUTO_WATERING_MODE;
#ifdef MANUAL_WATERING
void IRAM_ATTR ext_INT1_manualWatering() {
	wateringMode = MANUAL_WATERING_MODE;
}
#endif // MANUAL_WATERING



/*################################################################################
*
 * #---# setup() #---#
 *
 *################################################################################
*/

void setup() {
	bool setupFinished = false;
	while (!setupFinished) {
		//mpd.begin();
#ifdef TELEGRAM_BOT
		// # setup tg-bot #
		sClient.setCACert(TELEGRAM_CERTIFICATE_ROOT);
		gsm.tg_begin(&bot, BOT_CHATID);
		gsm.wifiConnect(WIFI_SSID, WIFI_PASSWORD);
#else
		gsm.begin();
#endif // TELEGRAM_BOT		


		// # setup GPIOS #
#ifdef WATER_PUMPS
		for (int i = 0; i < SIZEOF_ARRAY(WATER_PUMP_PINS); i++) {
			pinMode(WATER_PUMP_PINS[i], OUTPUT);
		}
#endif // WATER_PUMPS


#ifdef DHT_SENSORS
		for (int i = 0; i < SIZEOF_ARRAY(DHT_PINS); i++) {
			dht.setup(DHT_PINS[i], DHTesp::DHT11);
		}
#endif // DHT_SENSORS		


#ifdef SOIL_SENSORS
		// - initial analog reading (if not done, measurements won't be correct the first times) -
		//analogSetCycles(127);
		//for (int i = 0; i < SIZEOF_ARRAY(SOIL_SENSOR_PINS); i++) {
		//	//pinMode(SOIL_SENSOR_PINS[i], INPUT);
		//}

		////bool attach = adcAttachPin(SOIL_PIN);
		////analogSetAttenuation(ADC_11db);
		//analogReadResolution(12);
		//for (int i = 0; i < 3; i++) {
		//	gsm.soilHumidity = analogRead(SOIL_SENSOR_PINS[0]);
		//	delay(1000);
		//}
		//gsm.debugPrintln(attach ? "ADC-pin attached!" : "ADC-pin not attached!");
#endif // SOIL_SENSOR


#ifdef MANUAL_WATERING
		pinMode(MANUAL_WATERING_INPUT, INPUT_PULLUP);
		attachInterrupt(digitalPinToInterrupt(MANUAL_WATERING_INPUT), ext_INT1_manualWatering, FALLING);
#endif // MANUAL_WATERING


#ifdef FLOAT_SWITCH
		pinMode(FLOAT_SWITCH_INPUT, INPUT);
#endif // FLOAT_SWITCH


#ifdef TELEGRAM_BOT
		setupFinished = gsm.tg_send("Anzuchtstation_smol rebooted!");
#else
		setupFinished = true;
#endif // TELEGRAM_BOT


		if (setupFinished) {
			gsm.debugPrintln("# Setup finished #");
		}
		else {
			gsm.debugPrintln("# ERROR: Setup Error!#\nRestarting ESP...");
			ESP.restart();
		}
	}
}



/*################################################################################
*
 * #---# loop() #---#
 *
 *################################################################################
*/

void loop() {
	//bool loopTimer = mpd.delay(5000);
	if (mpd.init || mpd.delay(5000) || wateringMode == AUTO_WATERING_MODE) {
		// - measure temperature, humidity and soil-humidity until the dht returns no error -
		int errorCount = 0;
		do {
#ifdef DHT_SENSORS
			//while (!mpd_dht.delay(DHT22_SAMPLING, millis())) { delay(50); }
			//gsm.temperature = dht.getTemperature();
			gsm.dht_readTemperature(dht);
			//while (!mpd_dht.delay(DHT22_SAMPLING, millis())) { delay(50); }
			//gsm.humidity = dht.getHumidity();
			gsm.dht_readHumidity(dht);
			if (gsm.temperature == DHT_ERROR || gsm.humidity == DHT_ERROR) {
				errorCount++;
				gsm.debugPrint("\nError reading DHT values - ErrorCount: "); gsm.debugPrintln(errorCount);
			}
#endif // DHT_SENSORS


#ifdef SOIL_SENSORS
			gsm.soilHumidity = analogRead(SOIL_SENSOR_PINS[0]);
#endif // SOIL_SENSORS

			delay(100);

		} while (gsm.temperature == DHT_ERROR || gsm.humidity == DHT_ERROR);

		// - print out all the measurements -
		gsm.debugPrint("Air Temp (°C): "); gsm.debugPrintln(gsm.temperature); /*gsm.debugPrintln("°C");*/
		gsm.debugPrint("Air Humi (%): "); gsm.debugPrintln(gsm.humidity); /*gsm.debugPrintln("%");*/
#ifdef SOIL_SENSORS
		gsm.debugPrint("Soil Humi: "); gsm.debugPrint(gsm.soilHumidity); gsm.debugPrintln(" (mV)");
#endif // SOIL_SENSORS

		// - pump water when @soilHumidity is over @watering_treshold and only all @watering_frequencyH hours -
		unsigned int currentTime_H = gsm.millisToHours(millis());
		unsigned int pumpTimeTreshold_H = gsm.lastPump_H + gsm.watering_frequency_H;
		//gsm.debugPrint("Current Time (h): "); gsm.debugPrintln(currentTime_H); /*gsm.debugPrintln("h");*/
		//gsm.debugPrint("Pump time treshold (h): "); gsm.debugPrintln(pumpTimeTreshold_H); /*gsm.debugPrintln("h");*/

		int water = true;

#ifdef FLOAT_SWITCH
		//water = digitalRead(FLOAT_SWITCH_INPUT);
#endif // FLOAT_SWITCH


		if (water) {
			// - all watering statements (depending on config.h) -
#ifdef SOIL_SENSORS
			if ((currentTime_H >= pumpTimeTreshold_H && gsm.soilHumidity < gsm.watering_treshold) || wateringMode == MANUAL_WATERING_MODE) {
				gsm.pumpWater(0);
				wateringMode = AUTO_WATERING_MODE;
			}
#else
			if (currentTime_H >= pumpTimeTreshold_H || wateringMode == MANUAL_WATERING_MODE) {
				gsm.pumpWater();
				wateringMode = AUTO_WATERING_MODE;
			}
#endif // SOIL_SENSORS
		}
		else {
#ifdef TELEGRAM_BOT
			gsm.tg_send("Please refill me, senpai~");
#endif // TELEGRAM_BOT			
		}

#ifdef TELEGRAM_BOT
		gsm.wifiCheck(10);
		gsm.tg_handleMessages();
#endif // TELEGRAM_BOT
	}
	//delay(gsm.loop_delay);
	delay(500);
}
