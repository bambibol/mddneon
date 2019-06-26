#define PING_TIME 2000

long pingLastMillis;

void updatePing() {

  long ti = millis();
  ti = ti % PING_TIME; // 0..2000

  if (ti<pingLastMillis) {
    mqttClient.publish(pingCommandTopic, "ping");
  }



  pingLastMillis = ti; // 0..2000
}
