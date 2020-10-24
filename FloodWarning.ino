#include <SoftwareSerial.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include <ESP8266WiFi.h>

int status = WL_IDLE_STATUS;

WiFiServer server(80);

// ThingSpeak Settings
//char thingSpeakAddress[] = "api.thingspeak.com";
//unsigned long myChannelNumber = *****;
//String APIKey = "*************";             // enter your channel's Write API Key
//const int updateThingSpeakInterval = 20 * 1000; // 20 second interval at which to update ThingSpeak

// Variable Setup
long lastConnectionTime = 0;
boolean lastConnected = false;

// Initialize Arduino Ethernet Client
WiFiClient client;

String thingSpeakAddress= "http://api.thingspeak.com/update?";
String writeAPIKey;
String tsfield1Name;
String request_string;
HTTPClient http;

//Initialize pins
byte statusLed    = 13;

byte sensorInterrupt = 15;  // 0 = digital pin 2
byte sensorPin = 15;

// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 4.5;

volatile byte pulseCount;



unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;

//Initialize water level sensor values
int v;
int floatswitch0= 5;
int floatswitch1= 4;
int floatswitch2= 14;
int floatswitch3= 12;
int floatswitch4= 13;
void dump_radio_status_to_serialport(uint8_t);

//flowRATE
float  flowRate   = 0.0;

void setup()
{
{
  // Initialize a serial connection for reporting values to the host
  Serial.begin(9600);
  //delay(10);
  
  // Set up the status LED line as an output
  pinMode(statusLed, OUTPUT);
  digitalWrite(statusLed, HIGH);  // We have an active-low LED attached

  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);

  pulseCount        = 0;
  
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;

  // The Hall-effect sensor is connected to pin 2 which uses interrupt 0.

 attachInterrupt(digitalPinToInterrupt(sensorInterrupt), pulseCounter, FALLING);

  //Water level sensor inputs
  pinMode(5,INPUT);
  pinMode(4,INPUT);
  pinMode(14,INPUT);
  pinMode(12,INPUT);
  pinMode(13,INPUT);
  
  }

  // attempt to connect to Wifi network:
    Serial.begin(9600);
    WiFi.disconnect();
  WiFi.begin("WiFi Name","WiFi Password");
  Serial.println("Connecting to WIFi...");
  while ((!(WiFi.status() == WL_CONNECTED))){
    Serial.print(".");
    delay(300);
    Serial.print(".");
  }
  Serial.println("Connected");

}

/**
 * Main program loop
 */
void loop(){
{{
   if((millis() - oldTime) > 1000)    // Only process counters once per second
  {
    // Disable the interrupt while calculating flow rate and sending the value to the host
    detachInterrupt(sensorInterrupt);

    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;

    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    oldTime = millis();

    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;

    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;

    unsigned int frac;

    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate: ");
    Serial.print(flowRate);  // Print the integer part of the variable
    Serial.print("L/min");
    Serial.print("\t\n");           // Print tab space
    char buffer[25]; // char field
//String flowRate= String (flowRate);
dtostrf(flowRate, 2, 2, buffer); // http://dereenigne.org/arduino/arduino-float-to-string

    // Reset the pulse counter so we can start incrementing again
    pulseCount = 0;

    // Enable the interrupt again now that we've finished sending output

   attachInterrupt(digitalPinToInterrupt(sensorInterrupt), pulseCounter, FALLING);
  
  }
}
  //Display water level sensor readings
  int x=0;
  float v;
  Serial.print("Water level: ");
  if( x = digitalRead(13)){
   
    v=2.5;
    Serial.print(v);
    }
  else if ( x = digitalRead(12)){
   
    v=2.0;
    
    Serial.print(v);}
  else if ( x = digitalRead(14)){
   
    v=1.5;

    Serial.print(1.5);}
  else if ( x = digitalRead(4)){
  
    v=1.0;
    
    Serial.print(v);}
  else if ( x = digitalRead(5)){
  
    v=0.5;
    
    Serial.print(v);}
  else
    Serial.print(0);
  Serial.println("m");

  //Display flood warning status for each pulse
  int Alert;
    Serial.print("Status: ");
    if( /*x>=  2.0 || */v == 2.5){
      //if( flowRate >= 12 && flowRate <= 6)
      Alert=8;
        Serial.print("Warning imminent flood\n");
   }
     else if (/*x>= 1.5 ||*/ v==2.0){
     //if ( flowRate >= 5 && flowRate <= 10)
     Alert=6;
      Serial.print("Caution high levels\n");
  }
  else if ( /*x >= 1.0 || */v == 1.5){
    //if( flowRate >= 1 && flowRate <=4)
    Alert=3;
      Serial.print("Caution\n");
  }
  else{
    Serial.print("Safe\n");
    Alert=1;}
// Print Update Response to Serial Monitor
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  // Disconnect from ThingSpeak
  if (!client.connected() && lastConnected) {
    Serial.println("...disconnected");
    Serial.println();
    client.stop();
  }
  
  // Update ThingSpeak
  // ->Water level
  if (client.connect("api.thingspeak.com",80)) {
      writeAPIKey = "key=*************";
      tsfield1Name = "&field1=10";
      request_string = thingSpeakAddress;
      request_string += "key=";
      request_string += "*************";
      request_string += "&";
      request_string += "field1";
      request_string += "=";
      request_string += v;
      http.begin(request_string);
      http.GET();
      http.end();

    }

    //->Flow Rate
    if (client.connect("api.thingspeak.com",80)) {
      writeAPIKey = "key=************";
      tsfield1Name = "&field1=10";
      request_string = thingSpeakAddress;
      request_string += "key=";
      request_string += "************";
      request_string += "&";
      request_string += "field2";
      request_string += "=";
      request_string += flowRate;
      http.begin(request_string);
      http.GET();
      http.end();

    }
     //->Flow Rate
    if (client.connect("api.thingspeak.com",80)) {
      writeAPIKey = "key=************";
      tsfield1Name = "&field1=10";
      request_string = thingSpeakAddress;
      request_string += "key=";
      request_string += "************";
      request_string += "&";
      request_string += "field3";
      request_string += "=";
      request_string += Alert;
      http.begin(request_string);
      http.GET();
      http.end();

    }
    delay(1000);
}}

/*
Interrupt Service Routine
 */

void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}
