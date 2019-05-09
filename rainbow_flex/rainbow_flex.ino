/*


    LIBRARIES!


*/
#include <Adafruit_DotStar.h>
#include <WiFiNINA.h>
#include <MQTT.h>
#include <SPI.h>

#include <Adafruit_NeoPixel.h>

#define LED_PIN     6
#define NUMPIXELS  60
Adafruit_NeoPixel strip(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
#define BRIGHTNESS 255 // range of 0 - 255


/*


    WIFI THINGS!


*/
// IoT NETWORK
const char WIFI_SSID[] = "IoT"; // WiFI ssid
const char WIFI_PASS[] = "IoT4onderwijs"; //WiFI password

//// iPhone hotspot
//const char WIFI_SSID[] = "iPhone van Bambi"; // WiFI ssid
//const char WIFI_PASS[] = "gradient"; //WiFI password

// home wifi
//const char WIFI_SSID[] = "Verdieping 3 Koelkast"; // WiFI ssid
//const char WIFI_PASS[] = "Kutinternet123"; //WiFI password

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
//    Serial.println("yeah!");
//    Serial.println(payload);

    if (payload.equals("R")) { // make gradient (red to white ish) at key g
      rainbow(10);
    }

    else if (payload.equals("e")) { // key press 1
      for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
        strip.setPixelColor(i, strip.Color(255, 140, 0) );
      }
      strip.show(); // Update strip with new contents
    }

    else if (payload.equals("r")) { // key press r

    }

    else if (payload.equals("n")) { // key press n

    }

    else if (payload.equals("s")) { // key press s

    }

    else if (payload.equals("S")) { // key press S

    }

    else if (payload.equals("e")) { // key press e

    }

    else if (payload.indexOf("0x") == 0) {
      // color picker
      //      animationMode = ANIMATION_OFF;
      String hexString = payload.substring(2);
      Serial.print("hex received: ");
      Serial.println(hexString);

      char hexStringAsCharArray[6];
      hexString.toCharArray(hexStringAsCharArray, 7);
      Serial.println(hexStringAsCharArray);

      int rgb = (int)strtol(hexStringAsCharArray, NULL, 16);
      Serial.println(rgb / 256 / 256);
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
        strip.setPixelColor(i, g, r, b);
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
  //delay(1000);

  if (!wifiClient.connected()) {
    connect_to_mqtt();
  }

  //  rainbow(10);             // Flowing rainbow cycle along the whole strip

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
    delay(50);
  }

  // test GREEN
  Serial.println("Green test!");
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0, 255, 0);
    strip.show();
    delay(50);
  }

  // test BLUE
  Serial.println("Blue test!");
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0, 0, 255);
    strip.show();
    delay(50);
  }

  // test WHITE
  Serial.println("White test!");
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 255, 255, 255);
    strip.show();
    delay(50);
  }
  blank();
  Serial.println("ready for take off!");
}



// neopixel modes


// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
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
    delay(wait);  // Pause for a moment
  }
}

void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}
