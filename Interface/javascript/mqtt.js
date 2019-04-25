// MQTT LISTENER


// // Note: This line is only required when running in node.js
// var mqtt = require('mqtt');

$(function(){
  var client = mqtt.connect('wss://try:try@broker.shiftr.io', {
    clientId: 'javascript'
  });

  client.subscribe('/mddneon');

  client.on('connect', function(){
    console.log('client 2 has connected!');
  });

  client.on('message', function(topic, message) {
    console.log('new message:', topic, message.toString());
  });

  // buttons:

  $('#buttonR').click(function(){
    client.publish('/mddneon', 'R');
  })
  $('#buttonE').click(function(){
    client.publish('/mddneon', 'e');
  })
   $('#buttonr').click(function(){
    client.publish('/mddneon', 'r');
  })
    $('#buttons').click(function(){
    client.publish('/mddneon', 's');
  })
     $('#buttonS').click(function(){
    client.publish('/mddneon', 'S');
  })
      $('#buttonn').click(function(){
    client.publish('/mddneon', 'n');
  })

      $('#square').on("change", function(e) {
        //var t = target;
        var msg = '0x'+e.detail;
        //console.log(msg);
        client.publish('/mddneon', msg);
        //console.log("change!");
      });

})