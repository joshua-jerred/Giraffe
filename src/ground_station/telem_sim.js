const WebSocketClient = require("websocket").client;

var client = new WebSocketClient();

const telem = {
  source: "telemetry",
  type: "alert",
  body: {
    message: "test",
  },
};

client.on("connectFailed", function (error) {
  console.log("Connect Error: " + error.toString());
});

client.on("connect", function (connection) {
  console.log("Connection established!");
  function sendData() {
    console.log("Sending data...", connection.connected);
    if (connection.connected) {
      const data = JSON.stringify(telem);
      connection.sendUTF(data);
      setTimeout(sendData, 1000);
    }
  }
  connection.on("error", function (error) {
    console.log("Connection error: " + error.toString());
  });

  connection.on("close", function () {
    console.log("Connection closed!");
  });

  connection.on("message", function (message) {
    console.log("Current time on server is: '" + message.utf8Data + "'");
  });
  sendData();
});

client.connect("ws://localhost:8000/");
