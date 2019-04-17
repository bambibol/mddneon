// ********** LEDS **********

#include <Adafruit_DotStar.h>
#include <SPI.h>
#define NUMPIXELS 40 // Number of LEDs in strip
#define DATAPIN    6
#define CLOCKPIN   7
Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

// ********** WIFI **********

#include <WiFiNINA.h>
char ssid[] = "IoT";        // your network SSID (name)
char pass[] = "IoT4onderwijs";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                  // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;      //connection status
WiFiServer server(80);            //server socket

//special characters
char quote = '"';
char slash = '/';


void setup() {
  Serial.begin(57600);      // initialize serial communication
  // ********** LEDS **********

  strip.begin(); // Initialize pins for output
  //  strip.show(); // clear LEDs

  for ( int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, 0xff0000); // (0, 255, 0)); // turn all LEDs blue at startup
    strip.show();
  }

  // ********** WIFI **********

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);       // don't continue
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(2000);
  }
  server.begin();                           // start the web server on port 80
  printWiFiStatus();                        // you're connected now, so print out the status
}

// ********** LEDS **********

int      head  = 0, tail = -10; // Index of first 'on' and 'off' pixels
uint32_t color = 0xFF0000;      // 'On' color (starts red)


void loop() {

  // ********** WIFI **********

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }

  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            // the content of the HTTP response follows the header:
            client.println("<!DOCTYPE HTML>");
            client.print("<head>");
            client.print("<link rel=");
            client.print(quote);
            client.print("stylesheet");
            client.print(quote);
            client.print("href=");
            client.print(quote);
            client.print("http://bambiboland.nl/other/mddsign/style.css");  //NOTE: link to your own css stylesheet here
            client.print(quote);
            client.print(slash);
            client.print (">");
            client.print("</head>");
            client.print("<body>");
            client.println("<center><br><br><div class='container'><h1>CONTROL YOUR LIGHTS<h1/></div></center>");
            client.println("<center><div class='container'><left><button class='on' type='submit' value='ON' onmousedown=location.href='/H\'>ON</button>");
            client.println("<button class='off' type='submit' value='OFF' onmousedown=location.href='/L\'>OFF</button></div><br>");
            client.println("</body>");
            client.println("</html>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // ********** LEDS **********

        // Check to see if the client request was "GET /H" or "GET /L" or "GET /X":
        if (currentLine.endsWith("GET /H")) {
          // what happens when 'on' gets pressed:

          //          strip.setPixelColor(head, color); // 'On' pixel at head
          //          strip.setPixelColor(tail, 0);     // 'Off' pixel at tail
          //          strip.show();                     // Refresh strip
          //          delay(20);                        // Pause 20 milliseconds (~50 FPS)
          //
          //          if (++head >= NUMPIXELS) {        // Increment head index.  Off end of strip?
          //            head = 0;                       //  Yes, reset head index to start
          //            if ((color >>= 8) == 0)         //  Next color (R->G->B) ... past blue now?
          //              color = 0xFF0000;             //   Yes, reset to red
          //          }

          for ( int i = 0; i < NUMPIXELS; i++) {
            strip.setPixelColor(i, 0xffffff); // (0, 255, 0)); // turn all LEDs blue at startup
            strip.show();
          }
        }
        if (currentLine.endsWith("GET /L")) {
          // what happens when 'off' gets pressed:
          for ( int i = 0; i < NUMPIXELS; i++) {
            strip.setPixelColor(i, 0x000000); // (0, 255, 0)); // turn all LEDs blue at startup
            strip.show();
          }
        }

        delay(20);
      }
    }

    // ********** WIFI **********

    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}

// ********** WIFI **********

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}
