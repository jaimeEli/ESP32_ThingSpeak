#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "ThingSpeak.h"
#include <WiFi.h>

/*==========================================================
 * Defining ThingSpeak parameters
 * =========================================================
 */

#define SECRET_SSID ""    // replace MySSID with your WiFi network name
#define SECRET_PASS "" // replace MyPassword with your WiFi password

#define SECRET_CH_ID 0000000 // replace 0000000 with your channel number
#define SECRET_WRITE_APIKEY "XYZ"   // replace XYZ with your channel write API Key


char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
String myStatus = "";

/*=============================================================
 * Defining DHT22 parameters
 * ============================================================
 */
#define DHTPIN 15
#define DHTTYPE DHT22

DHT_Unified dht(DHTPIN,DHTTYPE);

uint32_t delayMS;

float temp, humid;

void setup() 
{
  Serial.begin(115200);
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);

  delayMS = sensor.min_delay / 1000;

  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak

}

void loop() 
{

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }
  
  delay(delayMS);

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature))
  {
    Serial.println("Error reading temperature!");
  }
  float temp = event.temperature;

  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity))
  {
    Serial.print("Error reading temperature");
  }
  float humid = event.relative_humidity;
  
  // set the fields with the values
  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, humid);

  // set the status
  ThingSpeak.setStatus(myStatus);

    // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }



  delay(19000); // Set the time between measures
}
