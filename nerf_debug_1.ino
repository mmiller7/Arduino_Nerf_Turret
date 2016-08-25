//nerf_debug_1
//Matthew Miller
//KK4NDE
//19-January-2013

/*
  This was some simple code I used to test the gun interface.
  I think I was  proving that the Arduino would loop fast enough
  to detect the darts passing thru the sensor and figure out
  approximate timing.  Unfortuniately I didn't comment it well.
*/

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);  
  pinMode(3,INPUT);  
  pinMode(2,OUTPUT);
  digitalWrite(2,LOW);
}

// the loop routine runs over and over again forever:
void loop() {
  if(digitalRead(3) == LOW && last == HIGH)
  {
    digitalWrite(13,HIGH);
    timekeeper=micros();
    last=LOW;
  }
  else
  {
    digitalWrite(13,LOW);
    if(last == LOW)
    {
      last=HIGH;
      long etime=micros()-timekeeper;
      Serial.print("eTime=");
      Serial.print(etime);
      Serial.print(" @ ");
      Serial.print((250000/etime));
      Serial.println("fps");
    }
  }
}
