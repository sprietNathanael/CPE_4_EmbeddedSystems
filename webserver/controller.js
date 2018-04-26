var sys = require('sys')

var exec = require('child_process').exec;

function Controller(socketControl, serialSender)
{
	this.socketControl = socketControl;
	this.serialSender = serialSender;
	this.direction = 90;
	this.serialSender.turn(this.direction);

	this.distanceReceived = (distance) => {
		this.socketControl.sendDistance(distance);
	}

	this.controlDirection = (order) => {
		switch(order)
		{
			case 'up':
				this.serialSender.goForward(120);
				break;
			case 'left':
				this.direction <=170 ? this.direction += 10 : null;
				this.serialSender.turn(this.direction);
				break;
			case 'down':
				this.serialSender.goBackward(70);				
				break;
			case 'right':
				this.direction >=10 ? this.direction -= 10 : null;
				this.serialSender.turn(this.direction);
				break;
			case 'stop':
				this.serialSender.stop();
				break;
		}
	}

	this.takePicture = () => {
		exec("fswebcam -d /dev/video0 /root/webserver/CPE_4_EmbeddedSystems/webserver/views/ressources/preview.jpg", (err, stdout, stderr)=>{});
	}

	setInterval(this.takePicture,3000);
}

module.exports = Controller;