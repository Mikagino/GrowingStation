/*
* DESCRIPTION: Class for delay objects (can be used to fake-multiprocess)
*/
class MP_Delay {
public:
	//void begin();
	bool delay(unsigned long milliseconds);

	bool init = true;
	unsigned long startTime = 0;
	unsigned long currentTime = 0;
};
