
#include <ESP8266WiFi.h>
#include "DHTesp.h"

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic


const char* host = "api.thingspeak.com";
const char* THINGSPEAK_API_KEY = "xxxxx";

// DHT Settings
#define DHTPIN 2     // what digital pin we're connected to. If you are not using NodeMCU change D6 to real pin
#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Update every 30 seconds = 0,5 minutes. Min with Thingspeak is ~20 seconds
const int UPDATE_INTERVAL_SECONDS = 60;


DHTesp dht;

void LEDOn(){
    digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
}

void LEDOff(){
  digitalWrite(2, LOW);   // turn the LED on (HIGH is the voltage level)  
}


void setup() {
  Serial.begin(115200);
  delay(10);

  WiFiManager wifiManager;
  wifiManager.autoConnect("IOT");
  Serial.println("");
  Serial.println("WiFi connection");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  dht.setup(2, DHTesp::DHT22); // Connect DHT sensor to GPIO 17

  // initialize LED
  pinMode(2, OUTPUT);
}

void loop() {      
//    Serial.println(host);
    
    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
      Serial.print("Unable to connect to host ");
      Serial.println(host);
      return;
    }

    LEDOn();
    
    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Can't receive data from DHT");
      delay(10);
      return;
    }
    
    // We now create a URI for the request
    String url = "/update?api_key=";
    url += THINGSPEAK_API_KEY;
    url += "&field1=";
    url += String(temperature);
    url += "&field2=";
    url += String(humidity);
    
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" + 
                 "Connection: close\r\n\r\n");
    delay(10);
    while(!client.available()){
      delay(100);
    }

    // Read all the lines of the reply from server and print them to Serial
//    while(client.available()){
//      String reply = client.readStringUntil('\r');
//    }
    
    Serial.println();
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print("C Humidity: "); 
    Serial.print(humidity);
    Serial.println("%"); 

    LEDOff();

    delay(1000 * UPDATE_INTERVAL_SECONDS);
}
