//nerfTurret
//Matthew Miller
//KK4NDE
//20-January-2013

/*
  This was the first fully functional Nerf Turret codebase.
  It controls a single gun which is activated by PIR motion
  detection.  It also monitors the break-beam sensor to
  identify when the gun has misfired or run out of darts
  and stop firing after 3 failures.  The timing delays are
  dynamically calculated to keep the program in sync such
  that the amount of time the code took to execute is
  accounted for in the delay.  This required a more complex
  delay by recording the "millis" time and then comparing
  it with a desired value at various intervals instead of
  calling a conventional sleep delay.
  
  It also interfaced thru the Ethernet shield to turn my
  bedroom LED lights (which are controlled by a simple
  webserver) on and off so I could more easily tell what
  the sketch was doing as I entered/exited the range of
  the PIR sensor.
*/

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {  0xDE, 0xAD, 0x54, 0xF6, 0x9D, 0x38 };
IPAddress lights(192,168,1,221);
IPAddress vlc(192,168,1,172);


#define pirPin 7 //PIR sensor logic pin
#define vccPin 8 //PIR sensor power pin

#define gunPin       2   //fire control pin
#define gunSense     3   //dart sensor pin
#define fireHold     200 //hold for one trigger press
#define fireTimeout  380 //max time to wait for dart to fire before calling misfire
#define cycleTimeout 400 //time for full gun fire cycle
#define misfireLimit 3   //max number of misfires in a row before critical error

int misfireCount=0;

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void setup() {
  //Serial.begin(9600);
  pinMode(vccPin,OUTPUT);
  pinMode(pirPin,INPUT);
  pinMode(gunPin,OUTPUT);
  pinMode(gunSense,INPUT);
  digitalWrite(gunPin,LOW);

  //Wait for device to settle
  digitalWrite(vccPin,LOW);
  delay(5000);
  //Power on sensor
  digitalWrite(vccPin,HIGH);
  //Wait for learning
  delay(25000); //25 second warmup

  //start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    // no point in carrying on, so do nothing forevermore:
    criticalError();
  }

  //ready
  turnOnGreen();
  delay(500);
  turnOff();
}

//fire control loop
void loop()
{
  if(digitalRead(pirPin) == HIGH) //if motion sensed
  {
    long time;
    
    
    //fire
    time=millis();
    digitalWrite(gunPin,HIGH);
    turnOnRed();
    while(millis()-time < fireHold); //delay (dynamic so code can insert above)
    digitalWrite(gunPin,LOW);

    //Monitor for misfire
    long timeoutTime=millis()+fireTimeout;
    while(true)
    {
      //Serial.println("Waiting for dart to pass through sensor");
      if(digitalRead(gunSense) == LOW)
      {
        //Serial.println("Found dart.");
        misfireCount=0;
        break;
      }
      else if(millis() > timeoutTime)
      {
        misfireCount++;
        //Serial.print("Misfire: if(");
        //Serial.print(misfireCount);
        //Serial.print(" > ");
        //Serial.print(misfireLimit);
        //Serial.println(")");
        if(misfireCount > misfireLimit)
          criticalError();
        break;
      }
    }

    
   
    turnOff();
    
    
    
    while((millis()-time) < cycleTimeout);
  }
}

//in case of critical error, flash PIR quickly
void criticalError()
{
      while(true)
    {
      digitalWrite(vccPin,HIGH);
      delay(100);
      digitalWrite(vccPin,LOW);
      delay(100);
    }
}

void turnOnRed()
{
  client.connect(lights, 80);
  // Make a HTTP request:
  client.println("GET /?r=255&g=0&b=0&noform HTTP/1.0");
  client.println();
  client.stop();
}

void turnOnGreen()
{
  client.connect(lights, 80);
  // Make a HTTP request:
  client.println("GET /?r=0&g=255&b=0&noform HTTP/1.0");
  client.println();
  client.stop();
}

void turnOff()
{
  client.connect(lights, 80);
  // Make a HTTP request:
  client.println("GET /?r=0&g=0&b=0&noform HTTP/1.0");
  client.println();
  client.stop();
}
