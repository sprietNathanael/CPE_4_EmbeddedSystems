const encoding = require ('encoding');

function SerialReceiver(serialPort, controller)
{
	this.controller = controller;
	this.port = serialPort;
	var chunk = "";

	this.port.on("open", function()
	{
		console.log("[Serial Receiver] Serial port opened");
	});

	this.port.on("data", (data) => {

		dataDec = encoding.convert(data, 'utf8', 'latin1');
		chunk += dataDec.toString();
		d_index = chunk.indexOf("\n");

		while(d_index > -1)
		{
			string = chunk.substring(0, d_index);
			this.computeFrame(string);
			

			chunk = chunk.substring(d_index+1); // Cuts off the processed chunk
			d_index = chunk.indexOf('\n'); // Find the new delimiter
		}
	});

	this.computeFrame = (string)=>
	{
		var opCode = string[0];
		switch(opCode)
		{
			case 'D':
				controller.distanceReceived(parseInt(string.substring(1)));
				// console.log("Distance : "+string.substring(1)+"mm");
				break;
			case 'S':
				// console.log("Car stopped");
				break;
			default:
				// console.log("Message received : ");
				// console.log(string);
		}
	}
}

module.exports = SerialReceiver;