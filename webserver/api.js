const express = require('express');
const expressLayouts = require("express-ejs-layouts");
const app = express();
app.use(expressLayouts);
app.set("view engine","ejs");

const SerialPort = require("serialport");
var serialPort = new SerialPort("/dev/ttyS4",
{
	baudRate: 115200,
	parser: SerialPort.parsers.raw
});

var SerialSender = require("./serialSender.js");
var serialeSender = new SerialSender(serialPort);

var server = require("http").Server(app);
var socketServer = require("socket.io")(server);
var SocketControl = require("./socket");
var socketControl = new SocketControl(socketServer);

var Controller = require("./controller.js");
var controller = new Controller(socketControl, serialeSender);

socketControl.initializeSocket(controller);

var SerialReceiver = require("./serialReceiver.js");
var serialReceiver = new SerialReceiver(serialPort, controller);

var sensors = [
	{
		name:"car 1",
		values : [
			{
				name: "val1",
				value: 50,
				unit: "%"
			},
			{
				name: "val2",
				value: 18,
				unit: "W"
			},
			{
				name: "val3",
				value: 150,
				unit: "m"
			}
		]
	},
	{
		name:"car 2",
		values : [
			{
				name: "val1",
				value: 85,
				unit: "%"
			}
		]
	},
	{
		name:"car 3",
		values : [
			{
				name: "val1",
				value: 23,
				unit: "%"
			},
			{
				name: "val2",
				value: 42,
				unit: "W"
			},
			{
				name: "val3",
				value: 1337,
				unit: "m"
			},
			{
				name: "val4",
				value: 0.58,
				unit: "A"
			}
		]
	},
	{
		name:"car 4",
		values : [
			{
				name: "val1",
				value: 0.5687,
				unit: "%"
			},
			{
				name: "val2",
				value: 987,
				unit: "W"
			}
		]
	}
]

app.get('/', function (req, res) {
  res.render("index.ejs", {sensors: sensors});
});

app.get('/control', function (req, res) {
  res.render("control.ejs");
})

app.use('*/ressources', express.static('views/ressources'));
app.use((req, res, next) => {
	res.setHeader("Content-Type", "text/plain");
	res.send(404, "Page introuvable !");
});

server.listen(8080);
server.listen("[Server] listening on 8080");