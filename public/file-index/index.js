function openDetails() {
	var allDetails = document.getElementsByTagName("details");
	for (var i = 0, len = allDetails.length; i < len; i++) {
		allDetails[i].setAttribute("open", "true");
	}
}

function closeDetails() {
	var allDetails = document.getElementsByTagName("details");
	for (var i = 0, len = allDetails.length; i < len; i++) {
		allDetails[i].removeAttribute("open");
	}
}