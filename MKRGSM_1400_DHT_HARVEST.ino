/*
 *  SORACOM x MKRGSM 1400
 *
 * Establish GSM data connection and send sensor data to SORACOM Harvest using HTTP.
 * 
 * Requirements:
 *  
 *  https://github.com/adafruit/DHT-sensor-library
 *  https://github.com/adafruit/Adafruit_Sensor
 *
 *
 */

// Include Arduino MKRGSM and Power Management libraries
#include "MKRGSM.h"
#include "ArduinoLowPower.h" 
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = "";

// initialize the library instance
GSM gsmAccess;        // GSM access: include a 'true' parameter for debug enabled
GPRS gprsAccess;  // GPRS access
GSMClient client;  // Client service for TCP connection

// messages for serial monitor response
String oktext = "OK";
String errortext = "ERROR";

// URL and path (for example: arduino.cc)
char url[] = "metadata.soracom.io";
char path[] = "/v1/subscriber.imsi";

// variable for save response obtained
String response = "";

// use a proxy
boolean use_proxy = false;

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
}

void loop() {
  use_proxy = false;

  // start GSM shield
  // if your SIM has PIN, pass it as a parameter of begin() in quotes
  Serial.print("Connecting GSM network...");
  if (gsmAccess.begin() != GSM_READY) {
    Serial.println(errortext);
    while (true);
  }
  Serial.println(oktext);

  // read APN introduced by user
  char apn[50] = "soracom.io";

  char login[50] = "sora";

  // read APN password introduced by user
  char password[20] = "sora";

  // attach GPRS
  Serial.println("Attaching to GPRS with your APN...");
  if (gprsAccess.attachGPRS(apn, login, password) != GPRS_READY) {
    Serial.println(errortext);
  } else {

    Serial.println(oktext);

    // connection with arduino.cc and realize HTTP request
    Serial.print("Connecting and sending GET request to metadata.soracom.io...");

//const char server[] = "metadata.soracom.io";
//const char resource[] = "/v1/subscriber.imsi";
//const int port = 80;

const char server[] = "cdn.rawgit.com";
const char resource[] = "/vshymanskyy/tinygsm/master/extras/logo.txt";
const int port = 80;

Serial.print(server);

  // Make a HTTP GET request:
  client.print(String("GET ") + resource + " HTTP/1.0\r\n");
  client.print(String("Host: ") + server + "\r\n");

  unsigned long timeout = millis();
  while (client.connected() && millis() - timeout < 10000L) {
    // Print available data
    while (client.available()) {
      char c = client.read();
      Serial.print(c);
      timeout = millis();
    }
  }
  Serial.println();

  //client.print("Connection: close\r\n\r\n");
  //client.stop();
  Serial.println("Server disconnected");
  while(true){
    delay(1000);
  }
  }


  delay(100000);

}

