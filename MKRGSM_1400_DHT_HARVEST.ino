/*
 *  SORACOM x MKR GSM 1400 x DHT Sensor
 *
 * Establish GSM data connection and send sensor data to SORACOM Harvest using HTTP JSON POST.
 * 
 * Requirements:
 *  
 *  https://github.com/arduino-libraries/MKRGSM
 *  https://github.com/adafruit/DHT-sensor-library
 *  https://github.com/adafruit/Adafruit_Sensor
 *
 */

// libraries
#include <MKRGSM.h>             // GSM library
#include <DHT.h>                // Adafruit DHT Sensor library

// PIN Number, none needed with Soracom SIM
// Soracom APN settings
const char PINNUMBER[] = "";
char apn[20] = "soracom.io";
char login[8] = "sora";
char password[8] = "sora";

// DHT Sensor settings
#define DHTPIN            0         // Pin which is connected to the DHT sensor.
// Uncomment the type of sensor in use:
#define DHTTYPE           DHT11     // DHT 11 
//#define DHTTYPE           DHT22     // DHT 22 (AM2302)
//#define DHTTYPE           DHT21     // DHT 21 (AM2301)

//Set to 1 if you would like temperature in Fahrenheit
#define FAHRENHEIT 0

// Initialize the library instance
GSM gsmAccess;    // GSM access: include a 'true' parameter for debug enabled
GPRS gprsAccess;  // GPRS access
GSMClient client; // Client service for TCP connection

// Initialise DHT Library
DHT dht(DHTPIN, DHTTYPE);

// Setting URL to http://harvest.soracom.io
// it also works with TCP but as we'd like to push two sensor values at the same time, 
// we'll use HTTP POST method, Harvest detects number in payloads for you could post a simple
// "number" message or otherwise (as we do in this example) a JSON payload with names and values
char url[] = "harvest.soracom.io";
char path[] = "/";


// Various variables used in this Sketch
String response = "";
String okmsg = "ok";
String errormsg = "error";

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(115200);
  
  // Wait for 1mins to see if Serial is connected, otherwise run the Sktech without Serial debugging
  int i;
  for(i=0; i<60; i++){
    if(Serial) break;
    delay(1000);
  }
}

void loop() {
  // start GSM shield
  // if your SIM has PIN, pass it as a parameter of begin() in quotes
  if(Serial) Serial.print("Connecting GSM network...");
  if (gsmAccess.begin(PINNUMBER) != GSM_READY) {
    if(Serial) Serial.println(errormsg);
    while (true);
  }else{
    if(Serial) Serial.println(okmsg);

    // Attach GPRS and check if it works
    if(Serial) Serial.println("Attaching to GPRS with your APN...");
    if (gprsAccess.attachGPRS(apn, login, password) != GPRS_READY) {
      if(Serial) Serial.println(errormsg);
    } else {
      // GPRS successfully attached 
      if(Serial) Serial.println(okmsg);

      // Reading temperature or humidity takes about 250 milliseconds!
      // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
      float h = dht.readHumidity();
      float t;
      if(FAHRENHEIT){
        t = dht.readTemperature(true);
      }else{
        t = dht.readTemperature();
      }

      // If Serial is connected, display current readings
      if(Serial) Serial.print("Humidity: ");
      if(Serial) Serial.print(h);
      if(Serial) Serial.println(" %\t");
      if(Serial) Serial.print("Temperature: ");
      if(Serial) Serial.println(t);

      // Prepare post data
      String PostData = "{\"Temperature\":"+ String(t) +",\"Humidity\":"+ String(h) +"}";

      // Connect to Soracom harvest
      if(Serial) Serial.print("Connecting and sending POST request to harvest.soracom.io...");
      int res_connect;

      res_connect = client.connect(url, 80);

      // If connection is successful, POST JSON data
      if (res_connect) {
        client.println("POST / HTTP/1.1");
        client.println("Host: harvest.soracom.io");
        client.println("User-Agent: Arduino/1.0");
        client.println("Connection: close");
        client.print("Content-Length: ");
        client.println(PostData.length());
        client.println();
        client.println(PostData);

        if(Serial) Serial.println(okmsg);

        // Read abd print the response from the server
        if(Serial) Serial.print("Receiving response...");
        boolean test = true;
        while (test) {
          // if there are incoming bytes available, print them
          if (client.available()) {
            char c = client.read();
            if(Serial) Serial.print(c);
          }

          // if the server's disconnected, stop the client:
          if (!client.connected()) {
            if(Serial) Serial.println("Shuttign down GSM connection: disconnected");
            client.stop();
            test = false;
          }
        }
      } else {
        // if we didn't get a connection to the server
        if(Serial) Serial.println(errormsg);
      }
    }
  }
  
  // Switching off GSM Access
  gsmAccess.shutdown();

  // Wait for 5mins until we send the next sensor reading
  delay(300000);
}
