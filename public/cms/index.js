var content = document.getElementById("content");

var action;

function connected(){
	goto("login.html");
}

var question;

function receivedPacket(e){
	action = JSON.parse(e.data);
	switch(action.type){
		case model.UPDATE_VIEW:
			content.innerHTML = action.data;
			break;
		case model.POST:
			document.getElementById("result").innerHTML = action.id;
			document.getElementById("title").value = action.title;
			document.getElementById("blog").value = action.text;
			break;
		case model.RESULT:
			document.getElementById("result").innerHTML = action.data;
			break;
		case "humanTest":
			question = action.data;
			setTimeout(function(){
				document.getElementById("testquestion").innerHTML = question;
			}, 100);
			break;
		default:
			console.log("Unknown Action!");
			console.log(action);
			break;
	}
}

function connectionClosed(){
	console.log("Your WebSocket connection dropped!");
}

function connectionError(){
	console.log("WebSocket connection error!");
	location.reload();
}

var ws = new WebSocket("wss://" + window.location.hostname + ":" + model.PORT + "/");
ws.onopen = connected;
ws.onmessage = receivedPacket;
ws.onclose = connectionClosed;
ws.onerror = connectionError;

function authenticate(){
	try{
		ws.send(JSON.stringify({type: model.AUTHENTICATE,
			username: document.getElementById("username").value,
			password: document.getElementById("password").value}));
	}catch(e){
		location.reload();
	}
}

function register(){
	try{
		ws.send(JSON.stringify({type: model.REGISTER,
			username: document.getElementById("username").value,
			password: document.getElementById("password").value,
			answer: document.getElementById("testanswer").value,
			}));
	}catch(e){
		location.reload();
	}
}

function goto(place){
	try{
		ws.send(JSON.stringify({type: model.GOTO,
			place: place}));
	}catch(e){
		location.reload();
	}
}

function edit(id){
	try{
		ws.send(JSON.stringify({type: model.EDIT_POST,
			id: id}));
	}catch(e){
		location.reload();
	}
}

function submitPost(){
	try{
		ws.send(JSON.stringify({type: model.POST,
			title: document.getElementById("title").value,
			text: document.getElementById("blog").value}));
	}catch(e){
		location.reload();
	}
}

function savePost(){
	try{
		ws.send(JSON.stringify({type: model.SAVE_POST,
			id: document.getElementById("result").innerHTML,
			title: document.getElementById("title").value,
			text: document.getElementById("blog").value}));
	}catch(e){
		location.reload();
	}
}