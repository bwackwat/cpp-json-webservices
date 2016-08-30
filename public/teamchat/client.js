navigator.getUserMedia = navigator.getUserMedia ||
navigator.webkitGetUserMedia ||
navigator.mozGetUserMedia ||
navigator.msGetUserMedia;

var video = document.getElementById("video");
var videoConfig = { video: true, audio: true };

function errBack(error) {
	alert("Come back to this page with a working webcam!");
}

if (navigator.getUserMedia) {
	navigator.getUserMedia(videoConfig, function (stream) {
		video.src = window.URL.createObjectURL(stream);
		video.play();
	}, errBack);
} else {
	alert("Come back to this page with a working webcam!");
}