const encoding = require ('encoding');
const SerialPort = require("serialport");
var serialPort = new SerialPort("/dev/ttyS4",
{
	baudRate: 115200,
	parser: SerialPort.parsers.raw
});

tosend = "test";
bufferToSend = encoding.convert(tosend, 'latin1');
serialPort.write(bufferToSend, function(err)
{
	if(err)
	{
		console.error("[Serial] "+err.message+" when trying to send "+tosend);
	}
});

var chunk = "";
serialPort.on("data", (data) => {

	dataDec = encoding.convert(data, 'utf8', 'latin1');
	chunk += dataDec.toString();
	d_index = chunk.indexOf("\n");

	while(d_index > -1)
	{
		string = chunk.substring(0, d_index);
		computeFrame(string);
		

		chunk = chunk.substring(d_index+1); // Cuts off the processed chunk
		d_index = chunk.indexOf('\n'); // Find the new delimiter
	}
});

function computeFrame(string)
{
	var opCode = string[0];
	switch(opCode)
	{
		case 'D':
			console.log("Distance : "+string.substring(1)+"mm");
			break;
		case 'S':
			console.log("Car stopped");
			break;
		default:
			// console.log("Message received : ");
			// console.log(string);
	}
}