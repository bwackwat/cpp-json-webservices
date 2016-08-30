var state = {};

function connectedToServer() {
}

function receivedServerMessage(e) {
	var action = JSON.parse(e.data);
	if (action.type === model.STATE) {
		state = action.data;
		console.log("Received state info from " + Object.keys(state).length + " player(s)!");
	}
}

function serverConnectionClosed() {
	//Closed connection :O
}

var ws = new WebSocket("wss://" + window.location.hostname + ":" + model.PORT + "/");
ws.onopen = connectedToServer;
ws.onmessage = receivedServerMessage;
ws.onclose = serverConnectionClosed;

window.onbeforeunload = function () {
	ws.close();
};

var action;
var keysDown = {};

function keydown(e) {
	if ((e.keyCode === model.MOVE_LEFT ||
		e.keyCode === model.MOVE_UP ||
		e.keyCode === model.MOVE_RIGHT ||
		e.keyCode === model.MOVE_DOWN) &&
		(typeof keysDown[e.keyCode] === 'undefined' ||
			keysDown[e.keyCode] === false)) {
		action = {};
		action.type = model.KEY_DOWN;
		action[model.KEY] = e.keyCode;
		ws.send(JSON.stringify(action));

		keysDown[e.keyCode] = true;
	}
}

function keyup(e) {
	if (e.keyCode === model.MOVE_LEFT ||
		e.keyCode === model.MOVE_UP ||
		e.keyCode === model.MOVE_RIGHT ||
		e.keyCode === model.MOVE_DOWN) {
		var action = {};
		action.type = model.KEY_UP;
		action[model.KEY] = e.keyCode;
		ws.send(JSON.stringify(action));

		keysDown[e.keyCode] = false;
	}
}

document.addEventListener("keydown", keydown);
document.addEventListener("keyup", keyup);

var canvas = document.getElementById("world");
var context = canvas.getContext('2d');

function repaintCanvas() {
	context.clearRect(0, 0, canvas.width, canvas.height);

	context.fillStyle = "black";

	for (var player in state) {
		context.fillRect(state[player].x,
			state[player].y,
			state[player].w,
			state[player].h);
	}
}

setInterval(repaintCanvas, 30);