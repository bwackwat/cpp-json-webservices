var left_delimiter = '(';
var item_delimiter = ' ';
var right_delimiter = ')';

var api = {};

function tokenize_some_lisp(code) {
	var some_tokenized_lisp = {};
	var next_index = 0;

	var current_lisp_item = "";
	var current_char;

	var inner_lisp = "";
	var inside_lisp = 0;

	for (var i = 0, len = code.length; i < len; i++) {
		current_char = code.charAt(i);

		if (current_char === '\t' || current_char === '\n' || current_char === '\r') {
			continue;
		}

		if (current_char === left_delimiter) {
			if (current_lisp_item !== "") {
				some_tokenized_lisp[next_index] = current_lisp_item;
				next_index++;
				current_lisp_item = "";
			}

			inside_lisp++;
			if (inside_lisp === 1) {
				continue;
			}
		}

		if (inside_lisp === 0) {
			if (current_char === item_delimiter) {
				if (current_lisp_item !== "") {
					some_tokenized_lisp[next_index] = current_lisp_item;
					next_index++;
					current_lisp_item = "";
				}
			} else {
				current_lisp_item += current_char;
				if (i === len - 1) {
					if (current_lisp_item !== "") {
						some_tokenized_lisp[next_index] = current_lisp_item;
						next_index++;
						current_lisp_item = "";
					}
				}
			}
			continue;
		}

		if (current_char == right_delimiter) {
			inside_lisp--;

			if (inside_lisp === 0) {
				//Commented lines allow for empty () items!
				//if(inner_lisp != "")
				//{
				some_tokenized_lisp[next_index] = tokenize_some_lisp(inner_lisp);
				inner_lisp = "";
				next_index++;
				//}
				continue;
			}
		}

		inner_lisp += current_char;
	}

	return some_tokenized_lisp;
}

function concatenate_lisp(lisp) {
	var result = "";
	for (var item in lisp) {
		result = result + lisp[item] + ' ';
	}
	return result;
}

function evaluate_some_lisp(lisp) {
	for (var item in lisp) {
		if (typeof (lisp[item]) === 'object') {
			lisp[item] = evaluate_some_lisp(lisp[item]);
		}
	}

	//console.log("Func " + lisp[0]);
	var func = lisp[0];
	delete lisp[0];

	if (func in api) {
		return api[func](lisp);
	} else {
		return func + ' ' + concatenate_lisp(lisp);
	}
}

function display_tokenized_lisp(lisp) {
	var display_html = "<table><tr>";
	for (var item in lisp) {
		if (typeof (lisp[item]) === 'object') {
			display_html += "<td>" + display_tokenized_lisp(lisp[item]) + "</td>";
		} else {
			display_html += "<td>" + lisp[item] + "</td>";
		}
	}
	return display_html + "</tr></table>";
}

function tokenize_lisp(lisp) {
	var tokenized_lisp = tokenize_some_lisp(lisp);

	console.log(tokenized_lisp);

	var table_output = document.getElementById("token-output");

	table_output.innerHTML = "<table><tr><td>" + display_tokenized_lisp(tokenized_lisp) + "</td></tr></table>";
}

function evaluate_lisp(lisp) {
	var tokenized_lisp = tokenize_some_lisp(lisp);

	var table_output = document.getElementById("token-output");

	table_output.innerHTML = "<table><tr><td>" + display_tokenized_lisp(tokenized_lisp) + "</td></tr></table>";

	//evaluate_some_lisp modifies tokenized_lisp, so order in this function matters.
	var evaluated_lisp = evaluate_some_lisp(tokenized_lisp);

	var output = document.getElementById("output");

	output.value = evaluated_lisp + "\n\n" + output.value;
}