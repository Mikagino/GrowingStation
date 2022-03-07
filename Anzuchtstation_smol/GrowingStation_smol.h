#include "DHTesp.h"

#define DHT_ERROR -1


class GrowingStation_smol{
  private:
  // - delays -
  const unsigned long dht_sampling = 3000;

  
  public:  
  int DHT_ReadTemperature(DHTesp dht);
  int DHT_ReadHumidity(DHTesp dht);
  int millisToHours(unsigned long millis);
};
