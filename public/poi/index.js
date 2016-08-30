window.onload = function(){

var localStorageLoginUsernameKey = "FLASK_MAP_EXAMPLE_USERNAME";
var localStorageLoginTokenKey = "FLASK_MAP_EXAMPLE_TOKEN";
var apiUrl = "https://" + window.location.hostname + "/api";

var status = document.getElementById("status");

//ELEMENTS

var login = document.getElementById("loginModal");
var loginResult = document.getElementById("loginResult");
var username = document.getElementById("username");
var password = document.getElementById("password");

var poi = document.getElementById("poiModal");
var poiLongitude = document.getElementById("poiLongitude");
var poiLatitude = document.getElementById("poiLatitude");
var poiLabel = document.getElementById("poiLabel");
var poiDescription = document.getElementById("poiDescription");

var signup = document.getElementById("signupModal");
var signupResult = document.getElementById("signupResult");
var signupUsername = document.getElementById("signupUsername");
var signupEmail = document.getElementById("signupEmail");
var signupPassword = document.getElementById("signupPassword");

//START MAP SCRIPT

map = new OpenLayers.Map("demoMap",{
	theme: false
});

map.addLayer(new OpenLayers.Layer.OSM(
	"OpenStreetMap", 
	[
		//Chrome will use HTTPS
		'//a.tile.openstreetmap.org/${z}/${x}/${y}.png',
		'//b.tile.openstreetmap.org/${z}/${x}/${y}.png',
		'//c.tile.openstreetmap.org/${z}/${x}/${y}.png'
	], 
	null));

var size = new OpenLayers.Size(21, 25);
var offset = new OpenLayers.Pixel(-(size.w/2), -size.h);
var icon = new OpenLayers.Icon("https://maps.google.com/intl/en_us/mapfiles/ms/micons/orange-dot.png", size, offset);

var markers = new OpenLayers.Layer.Markers("Markers");
map.addLayer(markers);
var currentmarker = null;

map.events.register("click", map, function(e) {
	if(localStorage.getItem(localStorageLoginTokenKey) === null){
		login.show();	
	}else{
		var position = map.getLonLatFromPixel(e.xy);

		if(currentmarker != null){
			markers.removeMarker(currentmarker);
		}
		currentmarker = new OpenLayers.Marker(position, icon.clone());
		markers.addMarker(currentmarker);
		
		position.transform(new OpenLayers.Projection("EPSG:900913"), new OpenLayers.Projection("EPSG:4326"));
		poiLongitude.innerHTML = position.lon.toFixed(5);
		poiLatitude.innerHTML = position.lat.toFixed(5);

		poi.show();
	}
});

map.zoomToMaxExtent();

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

function logout(){
	localStorage.removeItem(localStorageLoginTokenKey);
	status.innerHTML = "Welcome! You are not logged in; you cannot save points of interest.";
	markers.clearMarkers();
	poi.close();
}

function updateMarkers(){
	callAPI("/user/poi", {"token": localStorage.getItem(localStorageLoginTokenKey)}, function(response){
		if(typeof(response.error) === 'undefined'){
			markers.clearMarkers();
			for(var i = 0, len = response.result.length; i < len; i++){
				var geoloc = response.result[i].location;
				var lonlat = new OpenLayers.LonLat(geoloc.coordinates[0], geoloc.coordinates[1]);
				lonlat.transform(new OpenLayers.Projection("EPSG:4326"), new OpenLayers.Projection("EPSG:900913"));
				var newmarker = new OpenLayers.Marker(lonlat, icon.clone());
				markers.addMarker(newmarker);
			}
		}else{
			status.innerHTML = response.error;
			logout();
		}
	});
}

//LOGIN MODAL SCRIPT

function authSuccess(){
	status.innerHTML = "Welcome! You are logged in as " + localStorage.getItem(localStorageLoginUsernameKey);
	updateMarkers();
}

document.getElementById("loginButton").onclick = function() {
	callAPI("/login", {"login": username.value, "password": password.value}, function(response){
		if(typeof(response.error) === 'undefined'){
			localStorage.setItem(localStorageLoginUsernameKey, username.value);
			localStorage.setItem(localStorageLoginTokenKey, response.result.token);
			login.close();
			authSuccess();
		}else{
			loginResult.innerHTML = response.error;
		}
	});
};

document.getElementById("cancelLoginButton").onclick = function() {
	login.close();
};

if(localStorage.getItem(localStorageLoginTokenKey) === null){
	status.innerHTML = "Welcome! You are not logged in; you cannot save points of interest.";
	login.show();
}else{
	authSuccess();
}

//POI MODAL SCRIPT

document.getElementById("savePoi").onclick = function() {
	callAPI("/poi/new", {"token": localStorage.getItem(localStorageLoginTokenKey),
		"label": poiLabel.value,
		"description": poiDescription.value,
		"longitude": parseFloat(poiLongitude.innerHTML),
		"latitude": parseFloat(poiLatitude.innerHTML)}, function(response){
		if(typeof(response.error) === 'undefined'){
			poi.close();
			status.innerHTML = "Successfully saved the POI!";
			updateMarkers();
		}else{
			status.innerHTML = response.error;
		}
	});
	poiLabel.value = "";
	poiDescription.value = "";
};
document.getElementById("cancelPoi").onclick = function() {
	markers.removeMarker(currentmarker);
	poi.close();
	poiLabel.value = "";
	poiDescription.value = "";
};
document.getElementById("logout").onclick = function() {
	logout();
	poiLabel.value = "";
	poiDescription.value = "";
};

//SIGNUP MODAL SCRIPT

document.getElementById("signupButton").onclick = function() {
	callAPI("/signup", {"first_name": "poif", "last_name": "poil", "username": signupUsername.value, "email": signupEmail.value, "password": signupPassword.value}, function(response){
		if(typeof(response.error) === 'undefined'){
			signup.close();
			loginResult.innerHTML = "";
			username.value = signupUsername.value;
			password.value = signupPassword.value;
			status.innerHTML = "You signed up successfully!";
			login.show();
		}else{
			signupResult.innerHTML = response.error;
		}
	});
};
document.getElementById("cancelSignupButton").onclick = function() {
	signup.close();
};

document.getElementById("goToSignup").onclick = function() {
	login.close();

	signupUsername.value = username.value;
	signupPassword.value = password.value;

	signup.show();
};

}
