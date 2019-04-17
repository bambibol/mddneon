#include <Adafruit_DotStar.h>
#include <WiFiNINA.h>
#include <MQTT.h> 
#include <FastLED.h>

#define NUMPIXELS 300 // Number of LEDs in strip
#define DATAPIN    6
#define CLOCKPIN   7
Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

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
String pingCommandTopic = "mddneon";//devices/" + deviceId + "/commands/ping/run";


void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  if (topic.equals("mddneon")) {
    Serial.println("yeah!");
    Serial.println(payload);
    if (payload.equals("1")) {
      for ( int i = 0; i < NUMPIXELS; i++) {
        strip.setPixelColor(i, 0xff0000); // (0, 255, 0)); // turn all LEDs blue at startup
        
      }
      strip.show();
    } else if (payload.equals("2")) {
      for ( int i = 0; i < NUMPIXELS; i++) {
        strip.setPixelColor(i, 0x00ff00); // (0, 255, 0)); // turn all LEDs blue at startup
        
      }strip.show();
    } else if (payload.equals("3")) {
      for ( int i = 0; i < NUMPIXELS; i++) {
        strip.setPixelColor(i, 0x0000ff); // (0, 255, 0)); // turn all LEDs blue at startup
        
      }
      strip.show();
    } else if (payload.equals("0")) {
      for ( int i = 0; i < NUMPIXELS; i++) {
        strip.setPixelColor(i, 0x000000); // (0, 255, 0)); // turn all LEDs blue at startup
        
      }
      strip.show();
    }else if (payload.equals("9")) {
      for ( int i = 0; i < NUMPIXELS; i++) {
        strip.setPixelColor(i, 0xffffff); // (0, 255, 0)); // turn all LEDs blue at startup
        
      }
      strip.show();
    }
  }

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

  while (!mqttClient.connect("mddneon", mqttCloudUsername, mqttCloudPassword)) {
    Serial.print("*");
    delay(500);
  }

  Serial.println("Connected to MQTT");

  mqttClient.onMessage(messageReceived);

  mqttClient.subscribe(pingCommandTopic);
}

void setup() {
  Serial.begin(115200);

  strip.begin(); // Initialize pins for output
  for ( int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, 0xff0000); // (0, 255, 0)); // turn all LEDs blue at startup
      strip.show(); 
  }

}

void loop() {
  mqttClient.loop();
  delay(1000);

  if (!wifiClient.connected()) {
    connect();
  }

}
