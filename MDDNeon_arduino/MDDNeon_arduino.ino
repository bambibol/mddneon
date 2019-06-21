/*


    LIBRARIES & VARIABLES!


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
#define LED_TYPE    WS2811
#define COLOR_ORDER BGR
#define BRIGHTNESS 200 // range of 0 - 255

CRGB leds[NUMPIXELS];
#define FRAMES_PER_SECOND  120


int epilepsyValue = 0;

#define ANIMATION_OFF 0
#define ANIMATION_SMOOTH_RAINBOW 1
#define ANIMATION_METEOR_RAIN 2
#define ANIMATION_CONFETTI 3
#define ANIMATION_GLITTER_RAINBOW 4
#define ANIMATION_BOUNCY_BALL 5

int animationMode = ANIMATION_OFF;

/*


    WIFI THINGS!


*/
// IoT NETWORK
const char WIFI_SSID[] = "IoT"; // WiFI ssid
const char WIFI_PASS[] = "IoT4onderwijs"; //WiFI password


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

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUMPIXELS).setCorrection(TypicalLEDStrip);
  strip.begin(); // Initialize pins for output
  strip.show(); // turn all off
  strip.setBrightness(BRIGHTNESS);
  FastLED.setBrightness(BRIGHTNESS);

  int isconnected = wifi_connect(WIFI_SSID, WIFI_PASS);
  if ( WL_CONNECTED ==  isconnected) {
    on_wifi_success();
  }
}

uint8_t gHue = 0;


/*


      MESSAGE PARSING


*/
void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  // listen to mddneon channel
  if (topic.equals(pingCommandTopic)) {

    animationMode = ANIMATION_OFF;

    if (payload.equals("R")) { // key press R
      animationMode = ANIMATION_SMOOTH_RAINBOW;
    }

    else if (payload.equals("M")) { // key press e
      animationMode = ANIMATION_METEOR_RAIN;
    }

    else if (payload.equals("C")) { // key press n
      animationMode = ANIMATION_CONFETTI;
    }

    else if (payload.equals("G")) { // key press s
      animationMode = ANIMATION_GLITTER_RAINBOW;
    }

    else if (payload.equals("B")) { // key press S
      animationMode = ANIMATION_BOUNCY_BALL;
    }

    else if (payload.indexOf("0x") == 0) {

      animationMode = ANIMATION_OFF;

      // color picker
      String hexString = payload.substring(2);

      char hexStringAsCharArray[6];
      hexString.toCharArray(hexStringAsCharArray, 7);

      int rgb = (int)strtol(hexStringAsCharArray, NULL, 16);
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

    case ANIMATION_SMOOTH_RAINBOW:
      rainbowAlt();
      FastLED.show();
      FastLED.delay(1000 / FRAMES_PER_SECOND);
      EVERY_N_MILLISECONDS( 20 ) {
        gHue++;
      }
      break;

    case ANIMATION_METEOR_RAIN:
      meteorRain(0xff, 0xbe, 0x5c, 10, 64, true, 40);
      break;

    case ANIMATION_CONFETTI:
      confetti();
      FastLED.show();
      FastLED.delay(1000 / FRAMES_PER_SECOND);
      break;

    case ANIMATION_GLITTER_RAINBOW:
      rainbowWithGlitter();
      FastLED.show();
      FastLED.delay(1000 / FRAMES_PER_SECOND);
      break;

    case ANIMATION_BOUNCY_BALL:
      sinelon();
      FastLED.show();
      FastLED.delay(1000 / FRAMES_PER_SECOND);
      EVERY_N_MILLISECONDS( 20 ) {
        gHue++;
      }
      break;
  }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))


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


/* FAST LED LIBRARY MODES */

void rainbowAlt() {
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUMPIXELS, gHue, 7);
}

void rainbowWithGlitter() {
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbowAlt();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) {
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUMPIXELS) ] += CRGB::White;
  }
}

void confetti() {
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUMPIXELS, 10);
  int pos = random16(NUMPIXELS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon() {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUMPIXELS, 20);
  int pos = beatsin16( 13, 0, NUMPIXELS - 1 );
  leds[pos] += CHSV( gHue, 255, 192);
}
