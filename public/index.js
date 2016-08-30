window.onload = function(){

var localStorageLoginUsernameKey = "USERNAME_KEY";
var localStorageLoginTokenKey = "TOKEN_KEY";
var apiUrl = "https://" + window.location.hostname + "/api";

//ELEMENTS

var status = document.getElementById("status");
var username = document.getElementById("usernameField");
var password = document.getElementById("passwordField");
var loginButton = document.getElementById("loginButton");
var logoutButton = document.getElementById("logoutButton");

//API TOOLS

function callAPI(route, data, callback){
	var sendData = JSON.stringify(data);

	var http = new XMLHttpRequest();
	http.open("POST", apiUrl + route, true);
	http.setRequestHeader("Content-type", "application/json");
	http.onreadystatechange = function(){
		if(http.responseText == ""){
			//Bloody OPTIONS pre-flight...
			return;
		}
		console.log("RECV: " + http.responseText);
		var resjson = JSON.parse(http.responseText);
		if(http.readyState == 4 && http.status == 200){
			callback(resjson);
		}else if(http.readyState == 3){
			//Bogus OPTIONS response...
			
			//0: request not initialized
			//1: server connection established
			//2: request received
			//3: processing request
			//4: request finished and response is ready
		}else{
			//Invalid API usage...
			alert("HTTP ERROR!");
		}
	}
	http.send(sendData);
}

function checkLogin(){
	if(localStorage.getItem(localStorageLoginTokenKey) === null){
		status.innerHTML = "Not logged in.";
	
		username.style.display = "inline";
		password.style.display = "inline";
		loginButton.style.display = "inline";
		logoutButton.style.display = "none";
	}else{
		status.innerHTML = "Logged in as " + localStorage.getItem(localStorageLoginUsernameKey);

		username.style.display = "none";
		password.style.display = "none";
		loginButton.style.display = "none";
		logoutButton.style.display = "inline";
	}
}

document.getElementById("loginButton").onclick = function() {
	callAPI("/login", {"login": username.value, "password": password.value}, function(response){
		if(typeof(response.error) === 'undefined'){
			localStorage.setItem(localStorageLoginUsernameKey, username.value);
			localStorage.setItem(localStorageLoginTokenKey, response.result.token);
		}else{
			status.innerHTML = response.error;
		}
	});
	checkLogin()
};

document.getElementById("logoutButton").onclick = function() {
	localStorage.removeItem(localStorageLoginTokenKey);
	checkLogin();
}

checkLogin();

}





