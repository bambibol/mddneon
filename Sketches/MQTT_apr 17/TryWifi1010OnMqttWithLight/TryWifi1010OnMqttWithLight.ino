#include <Adafruit_DotStar.h>
#include <WiFiNINA.h>
#include <MQTT.h>
#include <FastLED.h>
#include <SPI.h>

#define NUMPIXELS 300 // Number of LEDs in strip
#define DATAPIN    11
#define CLOCKPIN   12
Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);
CRGB leds[300];

#define BRIGHTNESS 50 // range of 0 - 255

int      head  = 0, tail = -20; // Index of first 'on' and 'off' pixels
uint32_t color = 0xFF0000;      // 'On' color (starts red)



// ****** WIFI THINGS ******

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

// Topics
String pingCommandTopic = "mddneon";//devices/" + deviceId + "/commands/ping/run";


void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  // listen to mddneon channel
  if (topic.equals("mddneon")) {
    Serial.println("yeah!");
    Serial.println(payload);

    if (payload.equals(" ")) { // make gradient (red to white ish) at space bar
      for ( int i = 0; i < NUMPIXELS; i++) {
        int r = (int)max(0, min(255, map(i, NUMPIXELS * 0.00, NUMPIXELS * 0.33, 0, 255)));
        int g = (int)max(0, min(255, map(i, NUMPIXELS * 0.33, NUMPIXELS * 0.66, 0, 255)));
        int b = (int)max(0, min(255, map(i, NUMPIXELS * 0.66, NUMPIXELS * 1.00, 0, 255)));
        strip.setPixelColor(i, g, r, b);
      }
      strip.show();

    } else if (payload.equals("1")) {
      for ( int i = 0; i < NUMPIXELS; i++) {
        strip.setPixelColor(i, 0, 255, 0); // turn LEDs green at key 1

      } strip.show();
    } else if (payload.equals("2")) {
      for ( int i = 0; i < NUMPIXELS; i++) {
        strip.setPixelColor(i, 10, 200, 40); // turn LEDs pink at key 2

      } strip.show();
    } else if (payload.equals("3")) {
      for ( int i = 0; i < NUMPIXELS; i++) {
        strip.setPixelColor(i, 72, 47, 247);  // turn LEDs blue at key 3

      }
      strip.show();
    } else if (payload.equals("9")) {
      for ( int i = 0; i < NUMPIXELS; i++) {
        strip.setPixelColor(i, 255, 255 , 255); // turn LEDs white at key 9

      }
      strip.show();
    } else if (payload.equals("0")) {
      for ( int i = 0; i < NUMPIXELS; i++) {
        strip.setPixelColor(i, 0, 0, 0);// turn LEDs off at key 0

      }
      strip.show();
    }
  }
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

    // wait 2 seconds for connection:
    delay(2000);
  }
  Serial.print("\nconnecting...");

  Serial.println("\nconnected!\n");

  // when successfully connected to wifi, turn LEDs orange:
  for ( int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, 0xFFD700);
  }
  strip.show();

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
  strip.setBrightness(BRIGHTNESS);
  for ( int i = 0; i < 40; i++) {
    strip.setPixelColor(i, 10, 200, 40); // turn first 40 LEDs green at startup (one by one!)
    strip.show();
  }

  for ( int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, 10, 200, 40); // turn all LEDs green at startup (all at once!)
  }
  strip.show();

}

void loop() {
  mqttClient.loop();
  delay(1000);

  if (!wifiClient.connected()) {
    connect();
  }

}
