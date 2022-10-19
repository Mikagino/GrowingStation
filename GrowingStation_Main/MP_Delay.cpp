#include "MP_Delay.h"
#include <arduino-timer.h>

//GrowingStation_smol gsm2;


//void MP_Delay::begin() {
//    Serial.begin(9600);
//}


/*
* DESCRIPTION: check each calling of this function (each loop) if the difference between first and current call is bigger than the chosen delay (each delay needs a new object)
*/
bool MP_Delay::delay(unsigned long milliseconds) {
  if (init) {
    startTime = /*currentMillis*/millis();
    init = false;
    //Serial.println(startTime);
  }
  else {
    currentTime = /*currentMillis*/millis();
    //Serial.println(currentTime);
    if (currentTime - startTime >= milliseconds) {
      init = true;
      return true;
    }
  }
  return false;
}
