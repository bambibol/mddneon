import mqtt.*;

MQTTClient client;

function setup() {
  client = new MQTTClient(this);
  client.connect("mqtt://try:try@broker.shiftr.io", "Bambi");
  //client.subscribe("/testchannel");
  // client.unsubscribe("/example");
}


function draw() {
}

function keyPressed() {
  client.publish("/mddneon", ""+key); // publish keypress to mddneon channel 
}

function messageReceived(String topic, byte[] payload) {
  println("new message: " + topic + " - " + new String(payload));
}