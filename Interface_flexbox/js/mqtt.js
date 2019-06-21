// MQTT LISTENER
$(function() {
  var client = mqtt.connect('wss://try:try@broker.shiftr.io', {
    clientId: 'javascript'
  });
  client.subscribe('/mddneon');
  client.on('connect', function() {
    console.log('client 2 has connected!');
  });
  client.on('message', function(topic, message) {
    console.log('new message:', topic, message.toString());
  });
  // buttons:
  $('#buttonRainbow').click(function() {
    client.publish('/mddneon', 'R');
  })
  $('#buttonMeteor').click(function() {
    client.publish('/mddneon', 'M');
  })
  $('#buttonConfetti').click(function() {
    client.publish('/mddneon', 'C');
  })
  $('#buttonGlitter').click(function() {
    client.publish('/mddneon', 'G');
  })
  $('#buttonBouncy').click(function() {
    client.publish('/mddneon', 'B');
  })
  // $('#buttonn').click(function() {
  //   client.publish('/mddneon', 'n');
  // })
  // color picker:
  $('#square').on("change", function(e) {
    var msg = '0x' + e.detail;
    client.publish('/mddneon', msg);
  });
})

$(function toggle() {
  var x = document.getElementById("on");
  if (x.style.display === "none") {
    x.style.display = "block";
  } else {
    x.style.display = "none";
  }
  var y = document.getElementById("off");
  if (y.style.display === "block") {
    y.style.display = "none";
  } else {
    y.style.display = "block";
  }
})