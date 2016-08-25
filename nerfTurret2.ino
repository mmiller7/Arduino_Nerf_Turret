//nerfTurret2
//Matthew Miller
//KK4NDE
//23-January-2013

/*
  This was the second more elaborate Nerf Turret sketch
  used in the YouTube video.  It controls 2 guns as well
  as a laptop running VLC, and my LED lights; all
  activated by PIR motion detection.  It also monitors
  the break-beam sensors on both guns to sync them
  firing alternately (to increase rate of fire) and
  identify when each gun has misfired or run out of darts
  and stop firing after 3 failures.  The timing delays are
  dynamically calculated to keep the program in sync such
  that the amount of time the code took to execute is
  accounted for in the delay.  This required a more complex
  delay by recording the "millis" time and then comparing
  it with a desired value at various intervals instead of
  calling a conventional sleep delay.  This was especially
  complex because it was interleaving 2 guns which were
  running simaltaniously kept about 1/2 cycle out of sync.
  
  It  interfaced thru the Ethernet shield to turn my
  bedroom LED lights (which are controlled by a simple
  webserver) on and off.
  
  It interfaced thru the Ethernet shield to control the
  VLC playlist by sending telnet commands to VLC running
  on a nearby laptop.
*/


#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {  0xDE, 0xAD, 0x54, 0xF6, 0x9D, 0x38 };
IPAddress lights(192,168,1,221);
IPAddress vlc(192,168,1,104);
#define vlcPort 1234 //TCP port for VLC rc interface control


#define pirPin 7 //PIR sensor logic pin
#define vccPin 8 //PIR sensor power pin

#define gun1Pin      2   //fire control pin
#define gun1Sense    3   //dart sensor pin
#define fireHold     200 //hold for one trigger press
#define fireTimeout  180 //max time to wait (after fireHold ends) for dart to fire before calling misfire
#define cycleTimeout 400 //time for full gun fire cycle
#define misfireLimit 3   //max number of misfires in a row before critical error
#define gun2Pin      4   //fire control pin
#define gun2Sense    5   //dart sensor pin


bool last=LOW; //set last state of PIR detector (so we can act only on start/end)
bool network=true;
int misfireCount=0;
bool gunGood=true;
int misfire2Count=0;
bool gun2Good=true;

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void setup() {
  //Serial.begin(9600);
  pinMode(vccPin,OUTPUT);
  pinMode(pirPin,INPUT);
  pinMode(gun1Pin,OUTPUT);
  pinMode(gun1Sense,INPUT);
  digitalWrite(gun1Pin,LOW);
  pinMode(gun2Pin,OUTPUT);
  pinMode(gun2Sense,INPUT);
  digitalWrite(gun2Pin,LOW);
  
  //Wait for device to settle
  digitalWrite(vccPin,LOW);
  delay(5000);
  //Power on sensor
  digitalWrite(vccPin,HIGH);
  //Wait for learning
  delay(25000); //25 second warmup

  //start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    // if DHCP fails, drop network stuff.
    network=false;
  }

  //ready
  if(network)
  {
    turnOnGreen();
    delay(500);
    turnOff();
  }
  else
  {
    pinMode(13,OUTPUT);
    digitalWrite(13,HIGH);
    delay(500);
    digitalWrite(13,LOW);
  }
}

//fire control loop
void loop()
{
  //lots of free time here, just makes fire rate slower
  
  if(digitalRead(pirPin) == HIGH) //if motion sensed
  {
    //lots of free time here, just makes fire rate slower
    if(last == LOW && network)
    {
      last=HIGH;
      int toPlay=random(5,9);
      play(toPlay);
      delay(2000); //covers playtime of 5,6,7
      if(toPlay == 8)
        delay(2000); //additional playtime of 8
    }
    
    long time=0;
    long time2=0;
    long timeoutTime=0;
    
    //begin:fire1 start
    if(gunGood)
    {
      time=millis();
      digitalWrite(gun1Pin,HIGH);
    }
    //end:  fire1 start
    
    

    //about 140ms free here while gun1 is firing
    if(network && gun2Good)
      turnOnRed(); //flash corrisponding to gun 2
    
    
    //begin:Monitor 2 for misfire
    //must run without interruption
    if(gun2Good)
    {
      timeoutTime=millis()+fireTimeout;
      while(true)
      {
        if(digitalRead(gun2Sense) == LOW)
        {
          misfire2Count=0;
          break;
        }
        else if(millis() > timeoutTime)
        {
          misfire2Count++;
          if(misfire2Count > misfireLimit)
            gun2Good=false;
          break;
        }
      }
    }
    //must run without interruption
    //end:  Monitor 2 for misfire
    
    
    //about 20ms free here
    if(network)
      turnOff();
    
    //begin:fire1 stop
    while(millis()-time < fireHold); //delay (dynamic so code can insert above)
    digitalWrite(gun1Pin,LOW);
    //end:  fire1 stop
    //begin:fire2 start
    if(gun2Good)
    {
      time2=millis();
      digitalWrite(gun2Pin,HIGH);
    }
    //end:  fire2 start

    
    
    //about 140ms free here
    if(network && gunGood)
      turnOnRed(); //flash corrisponding to gun1
    
    
    
    //begin:Monitor for misfire
    //must run without interruption
    if(gunGood)
    {
      timeoutTime=millis()+fireTimeout;
      while(true)
      {
        if(digitalRead(gun1Sense) == LOW)
        {
          misfireCount=0;
          break;
        }
        else if(millis() > timeoutTime)
        {
          misfireCount++;
          if(misfireCount > misfireLimit)
            gunGood=false;
          break;
        }
      }
    }
    //must run without interruption
    //end:  Monitor for misfire

    
    //about 20ms free here
    if(network)
      turnOff();
    
    //begin:fire2 stop
    while(millis()-time2 < fireHold);
    digitalWrite(gun2Pin,LOW);
    //end:  fire2 stop
    
    
    //end of cycle
    while((millis()-time) < cycleTimeout);
  }
  else
  {
    if(last == HIGH && network)
    {
      last=LOW;
      if(gunGood && gun2Good)
        play(9);
      else
        play(10);
      delay(1000); //to cover play time of clips
    }
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

void play(int item)
{
  client.connect(vlc, vlcPort);
  // Send a telnet command:
  client.print("goto ");
  client.println(item);
  client.stop();
}
