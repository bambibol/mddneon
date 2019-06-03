import mqtt.*;

MQTTClient client;

void setup() {
  client = new MQTTClient(this);
  client.connect("mqtt://try:try@broker.shiftr.io", "Marcus");
  //client.subscribe("/testchannel");
  // client.unsubscribe("/example");
}

void draw() {}

void keyPressed() {
  client.publish("/testchannel", ""+key);
}

void messageReceived(String topic, byte[] payload) {
  println("new message: " + topic + " - " + new String(payload));
}
