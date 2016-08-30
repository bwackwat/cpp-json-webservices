var canvas = document.getElementById("world");
var context = canvas.getContext('2d');
var singleImage = context.createImageData(400, 200);

var ws = new WebSocket("wss://" + window.location.hostname + ":" + model.PORT + "/");
ws.binaryType = "arraybuffer";


ws.onmessage = function receivedServerMessage(e){
	var newdata = new Uint8ClampedArray(e.data);
	singleImage.data = newdata;
};

window.onbeforeunload = function(){
	ws.close();
};

var action;
var mouseDown = false;

var red = 0;
var green = 0;
var blue = 0;
var alpha = 255;

canvas.onmousedown = function(e){
	mouseDown = true;
};

canvas.onmousemove = function(e){
	if(mouseDown)	{
		action = {
			type: model.DRAW_PIXEL,
			pixel: {
				r: red,
				g: green,
				b: blue,
				a: alpha,
				x: e.x,
				y: e.y
			}
		};

		ws.send(JSON.stringify(action));
	}
};

canvas.onmouseup = function(e){
	mouseDown = false;
};

function repaintCanvas(){
	context.putImageData(singleImage, 0, 0);
}

setInterval(repaintCanvas, 30);