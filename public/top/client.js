var topdiv = document.getElementById("topdiv");

var ws = new WebSocket("wss://" + window.location.hostname + ":8006/");
ws.onmessage = function(e){
	topdiv.innerHTML = e.data;
};