#include "DHTesp.h"

#define DHT_ERROR -1


class GrowingStation_smol{
  private:
  // - delays (most of them in milliseconds) -
  unsigned long dht_sampling = 3000;
  unsigned long loop_delay = 5000;
  unsigned int watering_frequency = 24; // - how often it's watered (in hours) -
  unsigned long watering_delay_amount = 5000;

  // - measurements -
  int temperature = 0;
  
  public:  
  int DHT_ReadTemperature(DHTesp dht);
  int DHT_ReadHumidity(DHTesp dht);
  int millisToHours(unsigned long millis);
};
