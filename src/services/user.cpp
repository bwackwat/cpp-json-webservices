std::string tokencheck(JsonObject *json){
	JsonObject tokendata;
	try{
		GetTokenData(&tokendata, json);
	}catch(std::exception& e){
		return simple_error_json(e.what());
	}

	return "{\"result\":\"Token is good.\"}";
}

std::string login(JsonObject *json){
	pqxx::result res = MyApi::repo->GetUserByLogin(json->objectValues["username"]->stringValue);
	if(res.size() == 0){
		return simple_error_json("Username does not exist.");
	}

	std::string given_password_hashed = hash_password(json->objectValues["password"]->stringValue);
	if(given_password_hashed != res[0]["password"].c_str()){
		return simple_error_json("Incorrect password.");
	}
	
	std::stringstream token;
	token << "{\"id\":\"" << res[0]["id"].c_str();
	token << "\",\"username\":\"" << res[0]["username"].c_str();
	token << "\",\"proof\":\"" << given_password_hashed.substr(given_password_hashed.length() / 2);
	token << "\"}";

	std::stringstream response;
	response << "{\"result\":{\"token\":\"";
	response << encrypt_to_webtoken(token.str()) << "\"}}";

	return response.str();
}

std::string newuser(JsonObject *json){
	pqxx::result res = MyApi::repo->GetUserByUsernameOrEmail(
		json->objectValues["username"]->stringValue,
		json->objectValues["email"]->stringValue
	);
	
	if(res.size() != 0){
		return simple_error_json("Username already exists.");
	}

	if(json->objectValues["username"]->stringValue.length() < 8){
		return simple_error_json("Username must be at least 8 characters.");
	}
	if(json->objectValues["password"]->stringValue.length() < 8){
		return simple_error_json("Password must be at least 8 characters.");
	}
	if(json->objectValues["email"]->stringValue.length() < 8){
		return simple_error_json("I doubt that is your email address.");
	}
	if(json->objectValues["first_name"]->stringValue.length() < 2){
		return simple_error_json("I doubt that is your first name.");
	}
	if(json->objectValues["last_name"]->stringValue.length() < 2){
		return simple_error_json("I doubt that is your last name.");
	}
	
	res = MyApi::repo->CreateUser(
		json->objectValues["username"]->stringValue,
		hash_password(json->objectValues["password"]->stringValue),
		json->objectValues["email"]->stringValue,
		json->objectValues["first_name"]->stringValue,
		json->objectValues["last_name"]->stringValue
	);

	return "{\"result\":\"Successfully registered the user.\"}";
}

std::string users(JsonObject *json){
	JsonObject tokendata;
	try{
		GetTokenData(&tokendata, json);
	}catch(std::exception& e){
		return simple_error_json(e.what());
	}
	
	pqxx::result res = MyApi::repo->GetUsers();

	std::stringstream response;
	response << "{\"result\":[";
	for(pqxx::result::size_type i = 0; i < res.size(); i++){
		response << "{\"id\":\"" << res[i]["id"].c_str() << "\",";
		response << "\"username\":\"" << res[i]["username"].c_str() << "\",";
		response << "\"email\":\"" << res[i]["email"].c_str() << "\",";
		response << "\"first_name\":\"" << res[i]["first_name"].c_str() << "\",";
		response << "\"last_name\":\"" << res[i]["last_name"].c_str() << "\"}";
		if(i < res.size() - 1){
			response << ',';
		}
	}
	response << "}";

	return response.str();
}