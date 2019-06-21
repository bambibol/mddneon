/*


    LIBRARIES!


*/
#include <Adafruit_DotStar.h>
#include <WiFiNINA.h>
#include <MQTT.h>
#include <SPI.h>

#include "FastLED.h"
#include <Adafruit_NeoPixel.h>

#define LED_PIN     6
#define NUMPIXELS  100
Adafruit_NeoPixel strip(NUMPIXELS, LED_PIN, NEO_BGR + NEO_KHZ800); // GRB
#define BRIGHTNESS 200 // range of 0 - 255



/* rainbow stuff */

int rainbowColors[] = {
  0xff0000,
  0xff8800,
  0xffff00,
  0x00ff00,
  0x00FFFF,
  0x0000ff,
  0x8800ff,
  0xff00ff
};

int colorCount = 8;
int rainbowStartIndex = 0;
int epilepsyValue = 0;

#define ANIMATION_OFF 0
#define ANIMATION_ROTATING_RAINBOW 1
#define ANIMATION_OSCILLATING_RANDOMNESS 2
#define ANIMATION_SPARKLES 3
#define ANIMATION_SPARKLES_MINI 4
#define ANIMATION_EPILEPSY 5

int animationMode = ANIMATION_OFF;

/*


    WIFI THINGS!


*/
// IoT NETWORK
const char WIFI_SSID[] = "IoT"; // WiFI ssid
const char WIFI_PASS[] = "IoT4onderwijs"; //WiFI password

// iPhone hotspot
//const char WIFI_SSID[] = "iPhone van Bambi"; // WiFI ssid
//const char WIFI_PASS[] = "gradient"; //WiFI password


//WiFiSSLClient ipCloudStack;
WiFiClient wifiClient;
MQTTClient mqttClient;

int status = WL_IDLE_STATUS;

// Wia Cloud MQTT params
char mqttCloudServer[]     = "broker.shiftr.io"; //iot.eclipse.org";//"
int  mqttCloudPort         = 1883;

// get this from the wia dashboard. it should start with `d_sk`
char mqttCloudUsername[]   = "try";
char mqttCloudPassword[]   = "try";

// Topics
#define pingCommandTopic "mddneon"

/*


    SETUP!


*/

void setup() {
  Serial.begin(115200);
  while (!Serial) ; // wait untill serial connection is ready and kickin'

  strip.begin(); // Initialize pins for output
  strip.show(); // turn all off
  strip.setBrightness(BRIGHTNESS);

  int isconnected = wifi_connect(WIFI_SSID, WIFI_PASS);
  if ( WL_CONNECTED ==  isconnected) {
    on_wifi_success();
  }
}


/*


      MESSAGE PARSING


*/
void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  // listen to mddneon channel
  if (topic.equals(pingCommandTopic)) {

    animationMode = ANIMATION_OFF;

     if (payload.equals("R")) { // key press R
      animationMode = ANIMATION_ROTATING_RAINBOW;
    }

    else if (payload.equals("n")) { // key press n
      animationMode = ANIMATION_OSCILLATING_RANDOMNESS;
    }

    else if (payload.equals("s")) { // key press s
      //      animationMode = ANIMATION_SPARKLES
      SnowSparkle(0x10, 0x10, 0x10, 20, 200);
    }

    else if (payload.equals("S")) { // key press S
      animationMode = ANIMATION_SPARKLES_MINI;
    }

    else if (payload.equals("e")) { // key press e
      meteorRain(0xff, 0xbe, 0x5c, 10, 64, true, 40);
    }

    else if (payload.indexOf("0x") == 0) {

      animationMode = ANIMATION_OFF;

      // color picker
      String hexString = payload.substring(2);
      //      Serial.print("hex received: ");
      //      Serial.println(hexString);

      char hexStringAsCharArray[6];
      hexString.toCharArray(hexStringAsCharArray, 7);
      //      Serial.println(hexStringAsCharArray);

      int rgb = (int)strtol(hexStringAsCharArray, NULL, 16);
      //      Serial.println(rgb / 256 / 256);
      int r = rgb >> 16 & 0xff;
      int g = rgb >> 8 & 0xff;
      int b = rgb & 0xff;

      Serial.print("interpreted as: ");
      Serial.print("red=");
      Serial.print(r);
      Serial.print(", green=");
      Serial.print(g);
      Serial.print(", blue=");
      Serial.println(b);

      for ( int i = 0; i < NUMPIXELS; i++) {
        strip.setPixelColor(i, r, g, b);
      }
      strip.show();
    }

  }
}


/*

   CONNECT

*/

int wifi_connect(const char *ssid, const char *pass) {
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  // light up the on-board LED to signal that we are connected to wifi
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // print WiFi connection details
  Serial.println("Connected to wifi!");
  Serial.println("with IP address: ");
  Serial.println(WiFi.localIP());

  return WiFi.status();
}

void on_wifi_success() {
  // when connected to wifi, do a test R, G, B & W
  test_sequence();
}


void connect_to_mqtt() {

  // You need to set the IP address directly.
  mqttClient.begin(mqttCloudServer, mqttCloudPort, wifiClient);


  while (!mqttClient.connect(pingCommandTopic, mqttCloudUsername, mqttCloudPassword)) {
    Serial.print("*: returnCode=");
    Serial.print(mqttClient.returnCode());
    Serial.print(" lastError=");
    Serial.print(mqttClient.lastError());
    Serial.println();
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

  if (!wifiClient.connected()) {
    connect_to_mqtt();
  }

  switch (animationMode) {
    case ANIMATION_ROTATING_RAINBOW:
      rainbow();
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


void blank() {
  for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, strip.Color(0, 0, 0) );
  }
  strip.show(); // Update strip with new contents
}


void test_sequence() {

  // test RED
  Serial.println("Red test!");
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 255, 0 , 0);
    strip.show();
    delay(10);
  }

  // test GREEN
  Serial.println("Green test!");
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0, 255, 0);
    strip.show();
    delay(10);
  }

  // test BLUE
  Serial.println("Blue test!");
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0, 0, 255);
    strip.show();
    delay(10);
  }

  // test WHITE
  Serial.println("White test!");
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 255, 255, 255);
    strip.show();
    delay(10);
  }

  // turn entire strip off
  blank();
  Serial.println("rrrrrrrready for take off!");
}



/*


   MARCUS'S RANDOM MAGIC


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
    strip.setPixelColor(i, g, r, b); //
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
    strip.setPixelColor(i, g, r, b); //
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
    strip.setPixelColor(i, g, r, b); //
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


/*


   SOME FUN NEOPIXEL COLOR MODES


*/


// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow() {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
    for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(25);  // Pause for a moment
  }
}


void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {
  setAll(0, 0, 0);

  for (int i = 0; i < NUMPIXELS + NUMPIXELS; i++) {

    // fade brightness all LEDs one step
    for (int j = 0; j < NUMPIXELS; j++) {
      if ( (!meteorRandomDecay) || (random(10) > 5) ) {
        fadeToBlack(j, meteorTrailDecay );
      }
    }

    // draw meteor
    for (int j = 0; j < meteorSize; j++) {
      if ( ( i - j < NUMPIXELS) && (i - j >= 0) ) {
        setPixel(i - j, red, green, blue);
      }
    }

    showStrip();
    delay(SpeedDelay);
  }
}


void SnowSparkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay) {
  setAll(red, green, blue);

  int Pixel = random(NUMPIXELS);
  setPixel(Pixel, 0xff, 0xff, 0xff);
  showStrip();
  delay(SparkleDelay);
  setPixel(Pixel, red, green, blue);
  showStrip();
  delay(SpeedDelay);
}



void fadeToBlack(int ledNo, byte fadeValue) {
  uint32_t oldColor;
  uint8_t r, g, b;
  int value;

  oldColor = strip.getPixelColor(ledNo);
  r = (oldColor & 0x00ff0000UL) >> 16;
  g = (oldColor & 0x0000ff00UL) >> 8;
  b = (oldColor & 0x000000ffUL);

  r = (r <= 10) ? 0 : (int) r - (r * fadeValue / 256);
  g = (g <= 10) ? 0 : (int) g - (g * fadeValue / 256);
  b = (b <= 10) ? 0 : (int) b - (b * fadeValue / 256);

  strip.setPixelColor(ledNo, r, g, b);
}

void showStrip() {
  strip.show();
}

void setPixel(int Pixel, byte red, byte green, byte blue) {

  strip.setPixelColor(Pixel, strip.Color(red, green, blue));

}

void setAll(byte red, byte green, byte blue) {
  for (int i = 0; i < NUMPIXELS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}
