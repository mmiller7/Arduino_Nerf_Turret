//BedroomNerf
//Matthew Miller
//KK4NDE
//19-January-2013

/*
  This was an initial basic sketch to fire the Nerf gun using a PIR motion sensor.
  It also interfaced thru the Ethernet shield to turn my bedroom LED lights (which
  are controlled by a simple webserver) on and off so I could more easily tell
  what the sketch was doing as I entered/exited the range of the PIR sensor.
  
  The gist of it is when someone came into the room the lights turn on red and it
  will start shooting the Nerf gun until motion stops.  It does not take into
  account whether the gun has jammed or is out of darts since this is an early
  prototype program it ignores the sensor data.
*/

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {  0xDE, 0xAD, 0x54, 0xF6, 0x9D, 0x38 };
IPAddress server(192,168,1,221);

#define pirPin 7 //PIR sensor logic pin
#define vccPin 8 //PIR sensor power pin

#define gunPin 2 //fire control pin

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void setup() {
  pinMode(vccPin,OUTPUT);
  pinMode(pirPin,INPUT);
  pinMode(gunPin,OUTPUT);
  digitalWrite(gunPin,LOW);
  
  //Wait for device to settle
  digitalWrite(vccPin,LOW);
  delay(5000);
  //Power on sensor
  digitalWrite(vccPin,HIGH);
  //Wait for learning
  delay(25000); //25 second warmup

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      digitalWrite(vccPin,HIGH);
      delay(100);
      digitalWrite(vccPin,LOW);
      delay(100);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  turnOnGreen();
  delay(500);
  turnOff();
}

void loop()
{
  if(digitalRead(pirPin) == HIGH) //if motion sensed
  {
    digitalWrite(gunPin,HIGH);
    turnOnRed();
    delay(50);
    turnOff();
    delay(50);
  }
  else
    digitalWrite(gunPin,LOW);
}

void turnOnRed()
{
  client.connect(server, 80);
  // Make a HTTP request:
  client.println("GET /?r=255&g=0&b=0&noform HTTP/1.0");
  client.println();
  client.stop();
}

void turnOnGreen()
{
  client.connect(server, 80);
  // Make a HTTP request:
  client.println("GET /?r=0&g=255&b=0&noform HTTP/1.0");
  client.println();
  client.stop();
}

void turnOff()
{
  client.connect(server, 80);
  // Make a HTTP request:
  client.println("GET /?r=0&g=0&b=0&noform HTTP/1.0");
  client.println();
  client.stop();
}
