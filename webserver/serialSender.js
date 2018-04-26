const encoding = require ('encoding');

function SerialSender(serialPort)
{
	this.port = serialPort;

	this.port.on("open", function()
	{
		console.log("[Serial Sender] Serial port opened");
	});

	this.sendData = (data) =>
	{
		data+='\n';
		bufferToSend = encoding.convert(data, 'latin1');
		serialPort.write(bufferToSend, function(err)
		{
			if(err)
			{
				console.error("[Serial] "+err.message+" when trying to send "+tosend);
			}
			else
			{
				console.log("[Serial] Send message : "+data);
			}
		});
	}

	this.goForward = (coeff) =>
	{
		this.sendData("F"+coeff);
	}

	this.goBackward = (coeff) =>
	{
		this.sendData("F"+coeff);
	}

	this.turn = (direction) =>
	{
		this.sendData("A"+direction);
	}

	this.stop = () =>
	{
		this.sendData("F90");
	}

}

module.exports = SerialSender;