//nerfFireRateMeasurementCSV
//Matthew Miller
//KK4NDE
//20-January-2013

/*
  This was some simple code I used to test the gun interface.
  It waits for you to fire a dart (by manually pulling the
  trigger) or series of darts and prints out metrics about
  how fast they are being fired to the serial console in
  spreadsheet format which can be pasted into MS Excel
  to perform data analysis or create graphs.
*/

#define fire 2       //pin for fire trigger
#define sense 3      //pin for dart sensor beam

int count=0;
long last;

void setup()
{
  Serial.begin(9600);
  pinMode(fire,OUTPUT);
  pinMode(sense,INPUT);
  digitalWrite(fire,LOW);
  Serial.println("Dart,Cycle (milliseconds),Fire rate (darts/sec),Sensor uSec,Velocity ft/sec,Velocity meter/sec");
}

void loop()
{
  if(digitalRead(3) == LOW)
  {
    if(count == 0)
    {
      last=micros();
    }
    else
    {
      long time=micros();
      while(digitalRead(3) == LOW);
      long blocking=micros();
      Serial.print(count);

      Serial.print(',');
      float fireCycle=(time-last)/(float)1000;
      Serial.print(fireCycle);
      Serial.print(',');
      Serial.print(1000/fireCycle);
      Serial.print(',');
      long sensorTime=blocking-time;
      Serial.print(sensorTime);
      Serial.print(',');
      Serial.print((float)250000/sensorTime);
      Serial.print(',');
      Serial.println((float)76200/sensorTime);
      last=time;
    }
    count++;
    delay(100);
  }
}
