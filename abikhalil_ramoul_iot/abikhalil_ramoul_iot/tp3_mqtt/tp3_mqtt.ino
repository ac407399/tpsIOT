/*============================IoT==============================
  ============================TP MQTT===========================
  ==============Charbel ABI KHALIL and Inès RAMOUL==============*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

WiFiClient espClient;           // Wifi
PubSubClient client(espClient); // MQTT client
OneWire oneWire(23);
DallasTemperature tempSensor(&oneWire); //Set temperature sensor to pin D23


/*===== MQTT broker/server and TOPICS ========*/
const char* mqtt_server = "m16.cloudmqtt.com";
//const char* mqtt_server = "broker.shiftr.io";
/*"10.154.122.5"
  const char* mqtt_server = "192.168.1.23";*/

#define TOPIC_TEMP "miage1/menez/sensors/temp"
#define TOPIC_LED  "miage1/menez/sensors/led"

/*============= GPIO ======================*/
float temperatureValue;
float lightValue;
const int ledPin19 = 19;  //Pin D19 used for the red led
const int ledPin22 = 22;//Pin D23 used for the green led
const int photoResistor = A0;
int temperatureContract;


/*================ WIFI =======================*/
void print_connection_status() {
  Serial.print("WiFi status : \n");
  Serial.print("\tIP address : ");
  Serial.println(WiFi.localIP());
  Serial.print("\tMAC address : ");
  Serial.println(WiFi.macAddress());
}

void connect_wifi() {
 // const char* ssid = "HUAWEI-6EC2";
 // const char *password= "FGY9MLBL";
  //const char* ssid = "HUAWEI-553A";
  //const char *password = "QTM06RTT";
  //const char* ssid = "mowgli";
  //const char *password = "JWA9TNbf";
  //const char* ssid = "Bbox-12CAB6BF";
 // const char* password = "29295E9DDF62A672C9D4A34FA94D7F";
    const char* ssid = "AndroidAP";
    const char* password = "gemf4185";

  Serial.println("Connecting Wifi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Attempting to connect Wifi ..");
    delay(1000);
  }
  Serial.print("Connected to local Wifi\n");
  print_connection_status();
}

/*=============== SETUP =====================*/
void setup() {
  pinMode(ledPin19, OUTPUT);
  pinMode(ledPin22, OUTPUT);

  Serial.begin(9600);
  connect_wifi();

  client.setServer(mqtt_server, 13650);
  // set callback when publishes arrive for the subscribed topic
  client.setCallback(mqtt_pubcallback);
}

/*============== CALLBACK ===================*/
void mqtt_pubcallback(char* topic, byte* message,
                      unsigned int length) {
  // Callback if a message is published on this topic.

  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");

  // Byte list to String and print to Serial
  String messageTemp;
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic,
  // you check if the message is either "on" or "off".
  // Changes the output state according to the message
  if (String(topic) == TOPIC_LED) {
    Serial.print("Changing output to ");
    if (messageTemp == "on") {
      Serial.println("on");
      digitalWrite(ledPin19, HIGH); //Led rouge
    }
    else if (messageTemp == "off") {
      Serial.println("off");
      digitalWrite(ledPin19, LOW);
    }
    else if (messageTemp == "onchauffage") {
      Serial.println("chauffage on");
      digitalWrite(ledPin22, HIGH);  //Led verte
    }
    else if (messageTemp == "offchauffage") {
      Serial.println("chauffage off");
      digitalWrite(ledPin22, LOW); 
    }
  }
}

/*============= SUBSCRIBE =====================*/
void mqtt_mysubscribe(char *topic) {
  while (!client.connected()) { // Loop until we're reconnected
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("esp32", "xloreebd", "w5sTyyE-vwdA")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      //delay(5000);
    }
  }
}

/*========= Temperature =============*/
void temperature_sensor() {
  client.publish(TOPIC_TEMP, "----Temperature Value and Contract----");

  tempSensor.requestTemperaturesByIndex(0);
  temperatureValue = tempSensor.getTempCByIndex(0);
  lightValue = analogRead(photoResistor);

  //Si la chambre est occupée (lumiere> 800), la contrat de temperature (seuil) est fixé à 22 degrees.
  //Pendant la nuit, le seuil est 14.
  if (lightValue > 800) {
    temperatureContract = 25; //vous pouvez la changer, c'est juste un exemple
  } else {
    temperatureContract = 14;
  }

if (temperatureValue < temperatureContract )  {
  digitalWrite(ledPin22, HIGH);
  client.publish(TOPIC_TEMP,"La temperature de la chambre est plus basse que le contrat, Le chauffage est allume !");
}else {
    digitalWrite(ledPin22, LOW);
  }

  // Convert the value to a char array
  char tempString[8];
  dtostrf(temperatureValue, 1, 2, tempString);
  char seuil[8];
  dtostrf(temperatureContract, 1, 2, seuil);


  client.publish(TOPIC_TEMP, "La temperature de la chambre: ");
  client.publish(TOPIC_TEMP,tempString);
  client.publish(TOPIC_TEMP,"La valeur de la temperature du contrat: "); //seuil
  client.publish(TOPIC_TEMP,seuil);

  client.publish(TOPIC_TEMP, "----------End----------");
}

/*================= LOOP ======================*/
void loop() {
  int32_t period = 5000; // 5 sec
  /*--- subscribe to TOPIC_LED if not yet ! */
  if (!client.connected()) {
    mqtt_mysubscribe((char *)(TOPIC_LED));
  }

  /*--- Publish Temperature periodically   */
  delay(period);
  // MQTT Publish
  temperature_sensor();
  // Serial info
  //Serial.print("Published Temperature : "); Serial.println(tempString);

  client.loop(); // Process MQTT ... une fois par loop()
}
