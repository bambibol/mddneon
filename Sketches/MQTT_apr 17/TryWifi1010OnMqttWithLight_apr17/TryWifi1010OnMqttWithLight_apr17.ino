#include <Adafruit_DotStar.h>
#include <WiFiNINA.h>
#include <MQTT.h>
//#include <FastLED.h>
//#include <SPI.h>

#define NUMPIXELS 300 // Number of LEDs in strip
#define DATAPIN    11
#define CLOCKPIN   12
Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);
//CRGB leds[300];

#define BRIGHTNESS 20 // range of 0 - 255

int      head  = 0, tail = -20; // Index of first 'on' and 'off' pixels
uint32_t color = 0xFF0000;      // 'On' color (starts red)

//rgb
int rainbowColors[] = {
  0xff0000,
  0xff8800,
  0xffff00,
  0x00ff00,
  0x0000ff,
  0x8800ff,
  0xff00ff
};
int colorCount = 7;
int rainbowStartIndex = 0;
int epilepsyValue = 0;

#define ANIMATION_OFF 0
#define ANIMATION_ROTATING_RAINBOW 1
#define ANIMATION_OSCILLATING_RANDOMNESS 2
#define ANIMATION_SPARKLES 3
#define ANIMATION_SPARKLES_MINI 4
#define ANIMATION_EPILEPSY 5

int animationMode = ANIMATION_OFF;

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


/*


    SETUP!


*/

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


/*


      MESSAGE PARSING


*/


void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  // listen to mddneon channel
  if (topic.equals("mddneon")) {
    Serial.println("yeah!");
    Serial.println(payload);

    animationMode = ANIMATION_OFF;

    if (payload.equals(" ")) { // make gradient (red to white ish) at space bar
      for ( int i = 0; i < NUMPIXELS; i++) {
        int r = (int)max(0, min(255, map(i, NUMPIXELS * 0.00, NUMPIXELS * 0.33, 0, 255)));
        int g = (int)max(0, min(255, map(i, NUMPIXELS * 0.33, NUMPIXELS * 0.66, 0, 255)));
        int b = (int)max(0, min(255, map(i, NUMPIXELS * 0.66, NUMPIXELS * 1.00, 0, 255)));
        strip.setPixelColor(i, g, r, b);
      }
      strip.show();
    }

    else if (payload.equals("1")) {
      for ( int i = 0; i < NUMPIXELS; i++) {
        strip.setPixelColor(i, 0, 255, 0); // turn LEDs green at key 1

      } strip.show();
    }

    else if (payload.equals("2")) {
      for ( int i = 0; i < NUMPIXELS; i++) {
        strip.setPixelColor(i, 10, 200, 40); // turn LEDs pink at key 2

      } strip.show();
    }

    else if (payload.equals("3")) {
      for ( int i = 0; i < NUMPIXELS; i++) {
        strip.setPixelColor(i, 72, 47, 247);  // turn LEDs blue at key 3

      }
      strip.show();
    }

    else if (payload.equals("9")) {
      for ( int i = 0; i < NUMPIXELS; i++) {
        strip.setPixelColor(i, 255, 255 , 255); // turn LEDs white at key 9

      }
      strip.show();
    }

    else if (payload.equals("0")) {
      for ( int i = 0; i < NUMPIXELS; i++) {
        strip.setPixelColor(i, 0, 0, 0);// turn LEDs off at key 0

      }
      strip.show();
    }

    else if (payload.equals("r")) {
      drawRainbow();
    }

    else if (payload.equals("R")) {
      Serial.println(" ROTATING RAINBOW!" );
      animationMode = ANIMATION_ROTATING_RAINBOW;
    }

    else if (payload.equals("n")) {
      animationMode = ANIMATION_OSCILLATING_RANDOMNESS;
    }

    else if (payload.equals("s")) {
      animationMode = ANIMATION_SPARKLES;
    }

    else if (payload.equals("S")) {
      animationMode = ANIMATION_SPARKLES_MINI;
    }

    else if (payload.equals("e")) {
      animationMode = ANIMATION_EPILEPSY;
    }
  }
}







/*

   CONNECT

*/


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



/*


   LOOP


*/

void loop() {
  mqttClient.loop();
  //delay(1000);

  if (!wifiClient.connected()) {
    connect();
  }

  switch (animationMode) {
    case ANIMATION_ROTATING_RAINBOW:
      drawRotatingRainbow();
      break;
    case ANIMATION_OSCILLATING_RANDOMNESS:
      drawOscillatingRandomness();
      break;
    case ANIMATION_SPARKLES:
      drawRandomness(0.95f);
      break;
    case ANIMATION_SPARKLES_MINI:
      drawRandomness(0.995f);
      break;
    case ANIMATION_EPILEPSY:
      drawEpilepsy();
      break;
  }

}



/*

   RANDOMNESS

*/

void drawEpilepsy() {
  epilepsyValue = 255 - epilepsyValue;
  for ( int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, epilepsyValue, epilepsyValue, epilepsyValue); //
  }
  strip.show();
}

void drawOscillatingRandomness() {
  float cyclePercentage = (millis() % 5000) / 5000.0f; // 0..1
  float triangle = cyclePercentage < 0.5 ? (cyclePercentage * 2) : (1 - (cyclePercentage - 0.5) * 2); // 0..1..0
  drawRandomness(triangle);
}


void drawRandomness(float blackChance) {
  for ( int i = 0; i < NUMPIXELS; i++) {
    int col = getRainbowColor( (float)i / (NUMPIXELS - 1) );
    int r = (int)(random(255));
    int g = (int)(random(255));
    int b = (int)(random(255));

    if (random(10000.0f) < blackChance * 10000.0f) {
      r = g = b = 0;
    }
    strip.setPixelColor(i, g, r , b); //
  }
  strip.show();
}





/*


   RAINBOW STUFF


*/



void drawRainbow() {
  for ( int i = 0; i < NUMPIXELS; i++) {
    int col = getRainbowColor( (float)i / (NUMPIXELS - 1) );
    int r = col >> 16 & 0xff;
    int g = col >> 8 & 0xff;
    int b = col & 0xff;
    strip.setPixelColor(i, g, r , b); //
  }
  strip.show();
}

void drawRotatingRainbow() {
  for ( int i = 0; i < NUMPIXELS; i++) {
    int index = (i + rainbowStartIndex) % NUMPIXELS;
    int col = getRainbowColor( (float)index / (NUMPIXELS - 1) );
    int r = col >> 16 & 0xff;
    int g = col >> 8 & 0xff;
    int b = col & 0xff;
    strip.setPixelColor(i, g, r , b); //
  }
  strip.show();
  rainbowStartIndex++;
  rainbowStartIndex %= NUMPIXELS;
  Serial.println(rainbowStartIndex);
}

int getRainbowColor(float percentage)  {
  percentage = max(0, min(1, percentage));

  int index0 = (int)floor(colorCount * percentage);
  int index1 = min(colorCount - 1, index0 + 1);

  float p1 = colorCount * percentage - index0; //0: use col0, 1: use col1
  float p0 = 1 - p1;
  int col0 = rainbowColors[index0];
  int col1 = rainbowColors[index1];
  int r0 = col0 >> 16 & 0xff;
  int g0 = col0 >> 8 & 0xff;
  int b0 = col0 & 0xff;
  int r1 = col1 >> 16 & 0xff;
  int g1 = col1 >> 8 & 0xff;
  int b1 = col1 & 0xff;

  int r = (int)(r0 * p0 + r1 * p1);
  int g = (int)(g0 * p0 + g1 * p1);
  int b = (int)(b0 * p0 + b1 * p1);

  return r << 16 | g << 8 | b;
}
