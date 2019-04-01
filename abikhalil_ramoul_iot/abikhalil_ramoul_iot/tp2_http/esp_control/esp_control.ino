/*============================IoT==============================
 ============================TP HTTP===========================
 ==============Charbel ABI KHALIL and Inès RAMOUL==============*/

#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "ArduinoJson.h"
#include "AsyncJson.h"

const int ledPin = 19;
char* led = "";
int sensorValue;

OneWire oneWire(23);
DallasTemperature tempSensor(&oneWire);
 
const char* ssid = "AndroidAP";
const char* password = "gemf4185";
 
AsyncWebServer server(80);
 
void setup(){
  Serial.begin(9600);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println(WiFi.localIP());

   pinMode(ledPin, OUTPUT);

   tempSensor.begin();

 
 
  server.on("/allume", HTTP_GET, [](AsyncWebServerRequest *request){

     digitalWrite(ledPin, HIGH);
     //delay(1000);
     //digitalWrite(ledPin, LOW);
     //delay(1000);
    
    

    AsyncResponseStream *response = request->beginResponseStream("application/json");
          DynamicJsonBuffer jsonBuffer;
          JsonObject &root = jsonBuffer.createObject();

          root["led"] = "allumee";

          root.printTo(*response);
          response->addHeader("Access-Control-Allow-Origin", "*");
          request->send(response); 
  });

  server.on("/eteindre", HTTP_GET, [](AsyncWebServerRequest *request){

     digitalWrite(ledPin, LOW);
     //delay(1000);
     //digitalWrite(ledPin, LOW);
     //delay(1000);
    
    
     AsyncResponseStream *response = request->beginResponseStream("application/json");
          DynamicJsonBuffer jsonBuffer;
          JsonObject &root = jsonBuffer.createObject();

          root["led"] = "eteint";

          root.printTo(*response);
          response->addHeader("Access-Control-Allow-Origin", "*");
          request->send(response); 
  });

    server.on("/home", HTTP_GET, [](AsyncWebServerRequest *request){

     if (digitalRead(ledPin) == HIGH ){
      led = "allumée";
     }
     else {
      led = "eteinte";
     }

     float t;
     tempSensor.requestTemperaturesByIndex(0);
     t=tempSensor.getTempCByIndex(0);
     Serial.print(t);


    sensorValue = analogRead(A0);
      (sensorValue, DEC);
     
     

          AsyncResponseStream *response = request->beginResponseStream("application/json");
          DynamicJsonBuffer jsonBuffer;
          JsonObject &root = jsonBuffer.createObject();

          root["led"] = led;
          root["temperature"] = t;
          root["sensorValue"] = sensorValue;

          root.printTo(*response);
          response->addHeader("Access-Control-Allow-Origin", "*");
          request->send(response);  
              
            });

  
  server.begin();
}
 
void loop(){
}
