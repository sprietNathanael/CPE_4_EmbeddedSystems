const SerialPort = require("serialport");
serialPort = new SerialPort("/dev/ttyUSB0",
{
	baudrate: 115200,
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
})