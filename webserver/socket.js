function SocketControl(socketServer)
{
	this.socketServer = socketServer;
	this.createdSocket;
	this.controller;

	this.initializeSocket = (controller)=>{
		this.controller = controller;
		this.socketServer.on("connection", (socket)=>
		{
			this.createdSocket = socket;
			var socketId = socket.id;
			console.log("[Socket] Client "+socketId+" connected");
			socket.on("disconnect", ()=>{
				console.log("[Socket] Client "+socketId+" disconnected");
			});

			socket.on("test", (data)=>{
				console.log("[Socket] Test received : "+data.test);
				socket.emit("test",{test: "salut toi !"});
			});

			socket.on("controlDirection", (data) => {
				this.controller.controlDirection(data.direction);
			});
		});
	}

	this.sendDistance = (distance) => {
		if(this.createdSocket !== undefined)
		{
			this.createdSocket.emit("distance",{distance:distance});
		}
	}

}

module.exports = SocketControl;