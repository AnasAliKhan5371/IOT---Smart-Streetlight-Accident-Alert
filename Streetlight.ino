#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";
const char* mqtt_server = "mqtt_server_address";
const int mqtt_port = 1883;
const char* mqtt_user = "mqtt_username";
const char* mqtt_password = "mqtt_password";
const char* clientID = "your_client_id";

const int vehicleSensorPin = 2;
const int streetLightPin = 12;

WiFiClient espClient;
PubSubClient client(espClient);

volatile int vehicleCount = 0;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Handle MQTT messages if needed
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    if (client.connect(clientID, mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("street_light_control_topic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(vehicleSensorPin, INPUT_PULLUP);
  pinMode(streetLightPin, OUTPUT);
  
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Check vehicle sensor
  if (digitalRead(vehicleSensorPin) == LOW) {
    vehicleCount++;
    Serial.print("Vehicle Count: ");
    Serial.println(vehicleCount);
    delay(1000); // debounce time
  }

  // Sample logic to control street lights based on MQTT messages
  // Replace with your own logic
  if (client.connected()) {
    client.publish("vehicle_count_topic", String(vehicleCount).c_str());
    client.publish("street_light_status_topic", "ON");
  }

  // Example: Turn off street lights after 10 seconds of no vehicle detection
  if (vehicleCount == 0 || digitalRead(vehicleSensorPin) == HIGH) {
    digitalWrite(streetLightPin, LOW);
  } else {
    digitalWrite(streetLightPin, HIGH);
    delay(10000); // adjust as needed
  }
}