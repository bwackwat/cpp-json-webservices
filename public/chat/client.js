var chat = document.getElementById("chat");
var alias = document.getElementById("alias");
var message = document.getElementById("message");

function receivedServerMessage(e) {
	var newmessage = JSON.parse(e.data);
	chat.value = newmessage[model.ALIAS] + ": " + newmessage[model.MESSAGE] + "\n" + chat.value;
}

var ws = new WebSocket("wss://" + window.location.hostname + ":" + model.PORT + "/");
ws.onmessage = receivedServerMessage;

window.onbeforeunload = function () {
	ws.close();
};

function submitMessage() {
	var sendmessage = {};
	sendmessage.type = model.CHAT;
	sendmessage[model.ALIAS] = alias.value;
	sendmessage[model.MESSAGE] = message.value;

	message.value = "";
	message.focus();

	ws.send(JSON.stringify(sendmessage));
}

message.focus();

message.addEventListener("keydown", function (e) {
	if (e.keyCode === 13) {
		submitMessage();
	}
});