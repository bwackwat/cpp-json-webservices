api['+'] = function (parameters) {
	var result = 0;
	for (var param in parameters) {
		result = result + Number(parameters[param]);
	}
	return result;
};

api['-'] = function (parameters) {
	var result;
	for (var param in parameters) {
		if (typeof result === 'undefined') {
			result = Number(parameters[param]);
		} else {
			result = result - Number(parameters[param]);
		}
	}
	return result;
};

api['*'] = function (parameters) {
	var result = 1;
	for (var param in parameters) {
		result = result * Number(parameters[param]);
	}
	return result;
};

api['/'] = function (parameters) {
	var result;
	for (var param in parameters) {
		if (typeof result === 'undefined') {
			result = Number(parameters[param]);
		} else {
			result = result / Number(parameters[param]);
		}
	}
	return result;
};