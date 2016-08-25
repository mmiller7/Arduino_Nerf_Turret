//nerfFireRateMeasurement
//Matthew Miller
//KK4NDE
//20-January-2013

/*
  This was some simple code I used to test the gun interface.
  It waits for you to fire a dart (by manually pulling the
  trigger) or series of darts and prints out metrics about
  how fast they are being fired to the serial console in
  human-readable format.
*/

#define fire 2       //pin for fire trigger
#define sense 3      //pin for dart sensor beam

int count=0;
long timeStart;
long last;

void setup()
{
  Serial.begin(9600);
  pinMode(fire,OUTPUT);
  pinMode(sense,INPUT);
  digitalWrite(fire,LOW);
  Serial.println("Ready.");
}

void loop()
{
  if(digitalRead(3) == LOW)
  {
    if(count == 0)
    {
      timeStart=micros();
      last=micros();
    }
    else
    {
      long time=micros();
      while(digitalRead(3) == LOW);
      long blocking=micros();
      Serial.print("Dart ");
      Serial.println(count);

      Serial.print("Fire cycle: ");
      float fireCycle=(time-timeStart)/(count)/(float)1000;
      Serial.print(fireCycle);
      Serial.println(" milliseconds avg");
      Serial.print("Rate of Fire: ");
      Serial.print(1000/fireCycle);
      Serial.println(" darts/sec");
      Serial.print("Sensor uSec: ");
      long sensorTime=blocking-time;
      Serial.println(sensorTime);
      Serial.print("Velocity: ");
      Serial.print((float)250000/sensorTime);
      Serial.print(" ft/sec  (");
      Serial.print((float)76200/sensorTime);
      Serial.println(" meter/sec)");
      Serial.println("");
    }
    count++;
    delay(100);
  }
}
