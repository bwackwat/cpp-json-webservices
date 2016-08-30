function connected(){
	
}

function receivedPacket(e){
	var action = JSON.parse(e.data);
	switch(action.type){
		case 0:
			document.getElementById("result").innerHTML = action.data;
			if(action.success === true){
				var contactForm = document.getElementById("contactForm");
				contactForm.parentNode.removeChild(contactForm);
			}
			break;
		case "humanTest":
			document.getElementById("testquestion").innerHTML = action.data;
			break;
		default:
			alert("??: " + action);
			break;
	}
}

function connectionClosed(){
	console.log("You're WebSocket connection dropped!");
}

function connectionError(err){
	console.log(err);
}

var ws;

function connectToContactServer(){
	ws = new WebSocket("wss://" + window.location.hostname + ":" + 8005 + "/");
	ws.onopen = connected;
	ws.onmessage = receivedPacket;
	ws.onclose = connectionClosed;
	ws.onerror = connectionError;
}

function submitMessage(){
	try{
		ws.send(JSON.stringify({type: 2,
			fromname: document.getElementById("fromname").value,
			fromemail: document.getElementById("fromemail").value,
			subject: document.getElementById("subject").value,
			message: document.getElementById("message").value,
			answer: document.getElementById("testanswer").value}));
	}catch(err){
		console.log(err);
	}
}