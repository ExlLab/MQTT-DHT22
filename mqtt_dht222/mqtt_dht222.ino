/* Created by Exl2Lab, ‎July ‎22, ‎2018.
  1. Schematic:
    - DHT22 leg 1 - VCC (3.3V)
    - DHT22 leg 2 - GPIO2
    - DHT22 leg 4 - GND

  2. Configuration (Home assistant):
  sensor temp:
      platform: mqtt
      state_topic: 'dht22/temp'
      name: 'Temperature'
      unit_of_measurement: '°C'

  sensor humidity:
      platform: mqtt
      state_topic: 'dht22/humidity'
      name: 'Humidity'
      unit_of_measurement: '%'
  
  3. Note: Edit lines 27-33 for your config.
*/
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <DHT.h>

// WiFi
const char* wifiSSID = "SSID";  //your SSID WiFI
const char* wifiPassword = "pw123456789";  // your password WiFi

// MQTT
const char* mqttServer = "192.168.1.100";  	//Your IP Hass
const char* mqttUsername = "hass";			//mqtt user 
const char* mqttPassword = "hass";			//mqtt password
const char* mqttClientId = "ESP8266-DHT22";
const char* temp_topic = "dht22/temp";
const char* humidity_topic = "dht22/humidity";

// DHT
const int pushData_in_seconds = 2; 
#define DHT_Pin 2			//GPIO2
#define DHTTYPE DHT22
DHT dht(DHT_Pin, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
delay(100);
   
  digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(115200);
  setupWifi();
  client.setServer(mqttServer, 1883);

  // Set up the DHT sensor
  dht.begin();
 
}

void setupWifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifiSSID);

  WiFi.begin(wifiSSID, wifiPassword);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqttClientId, mqttUsername, mqttPassword)) {
      Serial.println("connected!!!");
	  client.subscribe(temp_topic);
      client.subscribe(humidity_topic);
    } else {
      Serial.print("mqtt failed!");
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

static  int i;
void loop() {
    
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // Wait a few seconds between measurements.
  delay(200);
  float humidity_value = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float temp_value = dht.readTemperature();

  if (isnan(humidity_value) || isnan(temp_value)) {
    Serial.println("ERROR: Failed to read from DHT sensor!");
    i=i+1;
    Serial.println(i);
    if (i==12) {
      i=0;
      ESP.reset();
    }
    return;
  } else {
    Serial.println(String(temp_value) + " | " + String(humidity_value));
	
	client.publish(temp_topic, String(temp_value).c_str());
	client.publish(humidity_topic, String(humidity_value).c_str());
    delay(1000 * pushData_in_seconds); 
  }
}
