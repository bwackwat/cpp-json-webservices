api.alert = function (parameters) {
	alert(concatenate_lisp(parameters));
	return '';
};

api.ajax = function (parameters) {
	var xmlhttp = new XMLHttpRequest();
	var result = "";
	for (var param in parameters) {
		xmlhttp.open("GET", parameters[param], false);
		xmlhttp.send();
		result = result + xmlhttp.responseText;
	}
	return result;
};