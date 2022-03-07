#include "GrowingStation_smol.h"


int GrowingStation_smol::DHT_ReadTemperature(DHTesp dht) {
  int temperature = round(dht.getTemperature());

  if (dht.getStatus() != 0) {
    return DHT_ERROR;
  }
  else {
    return temperature;
  }
}


int GrowingStation_smol::DHT_ReadHumidity(DHTesp dht) {
  int humidity = dht.getHumidity();

  if (dht.getStatus() != 0) {
    return DHT_ERROR;
  }
  else {
    return humidity;
  }
}


int GrowingStation_smol::millisToHours(unsigned long millis){
  return round(millis/1000/60/60);
}
