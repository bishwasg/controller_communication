#include <SCServo.h>

/*
 *  
 */
SMSCL sm;

void setup()
{
  Serial.begin(115200);
	Serial2.begin(115200);
	sm.pSerial = &Serial2;
	sm.wheelMode(1);
}

void loop()
{ 
  sm.WriteSpe(1, 3000);
  u8 Err;
  s16 pos = sm.ReadPos(1, &Err);
  if(Err==0)
  {
    Serial.println(pos, DEC);
    delay(10);

//    if(pos < 2000) sm.WriteSpe(1, 1000);
//    else sm.WriteSpe(1, -1000);
  }
  else
  {
    delay(2000);
  }
   
//	delay(6000);
//	sm.WriteSpe(1, 0);
//	delay(6000);
//	sm.WriteSpe(1, -500);
//	delay(6000);
//	sm.WriteSpe(1, 0);
//	delay(6000);
}
