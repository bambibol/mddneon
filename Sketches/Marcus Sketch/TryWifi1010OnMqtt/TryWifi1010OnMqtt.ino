#include <WiFiNINA.h>
#include <MQTT.h>

const char WIFI_SSID[] = "IoT"; // WiFI ssid 
const char WIFI_PASS[] = "IoT4onderwijs"; //WiFI password

//WiFiSSLClient ipCloudStack;
WiFiClient wifiClient;
MQTTClient mqttClient;

int status = WL_IDLE_STATUS;

// Wia Cloud MQTT params
char mqttCloudServer[]     = "broker.shiftr.io";
int  mqttCloudPort         = 1883;

// get this from the wia dashboard. it should start with `d_sk`
char mqttCloudUsername[]   = "try"; 
char mqttCloudPassword[]   = "try"; 

// Wia API parameters
char server[] = "broker.shiftr.io";
const int mqttPort = 1883;  // Default MQTT port

//const String deviceId = "your-device-id";   // starts with dev_, found in Wia Dashboard

// Topics
String pingCommandTopic = "testchannel";//devices/" + deviceId + "/commands/ping/run";


void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
//  if (topic.equals(pingCommandTopic)) {
//    Serial.println("Sending ping...");
//    Serial.println("Pinged!"); 
//    // Start rotating
//    // wait 5 seconds for connection:
//    delay(5000);
//  }
}

void connect() {
  
 // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(WIFI_SSID);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(WIFI_SSID, WIFI_PASS);

    // wait 5 seconds for connection:
    delay(5000);
  }
  Serial.print("\nconnecting...");

  Serial.println("\nconnected!\n");
  Serial.print("\nIP address: ");
  Serial.println(WiFi.localIP());

  // You need to set the IP address directly.
  mqttClient.begin(mqttCloudServer, mqttCloudPort, wifiClient);

  Serial.println("start wia connect"); Serial.println();
  
  while (!mqttClient.connect("testchannel", mqttCloudUsername, mqttCloudPassword)) {
    Serial.print("*");
    delay(500);    
  }

  Serial.println("Connected to MQTT");

  mqttClient.onMessage(messageReceived);
  
  mqttClient.subscribe(pingCommandTopic);
}

void setup() {
  Serial.begin(115200);

}

void loop() {
  mqttClient.loop();
  delay(1000);
  
  if (!wifiClient.connected()) {
    connect();
  }

}
