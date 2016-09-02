std::string login(Document *json){
	pqxx::result res = MyApi::repo->GetUserByLogin((*json)["username"].GetString());
	if(res.size() == 0){
		return simple_error_json("Username does not exist.");
	}

	std::string given_password_hashed = hash_password((*json)["password"].GetString());
	if(given_password_hashed != res[0]["password"].as<const char *>()){
		return simple_error_json("Incorrect password.");
	}
	
	StringBuffer token_buffer;
	Writer<StringBuffer> token_writer(token_buffer);

	token_writer.StartObject();
	token_writer.String("id");
	token_writer.Int(res[0]["id"].as<int>());
	token_writer.String("username");
	token_writer.String(res[0]["username"].as<const char *>());
	token_writer.String("proof");
	token_writer.String(given_password_hashed.substr(given_password_hashed.length() / 2).c_str());
	token_writer.EndObject();

	StringBuffer response_buffer;
	Writer<StringBuffer> writer(response_buffer);

	writer.StartObject();
	writer.String("result");
	writer.StartObject();
	writer.String("token");
	writer.String(encrypt_to_webtoken(token_buffer.GetString()).c_str());
	writer.EndObject();
	writer.EndObject();

	return response_buffer.GetString();
}	

std::string newuser(Document *json){
	pqxx::result res = MyApi::repo->GetUserByUsernameOrEmail(
		(*json)["username"].GetString(),
		(*json)["email"].GetString()
	);
	
	if(res.size() != 0){
		return simple_error_json("Username already exists.");
	}
	
	res = MyApi::repo->CreateUser(
		(*json)["username"].GetString(),
		hash_password((*json)["password"].GetString()),
		(*json)["email"].GetString(),
		(*json)["first_name"].GetString(),
		(*json)["last_name"].GetString()
	);

	StringBuffer response_buffer;
	Writer<StringBuffer> writer(response_buffer);

	writer.StartObject();
	writer.String("result");
	writer.StartObject();
	writer.String("id");
	writer.String(res[0]["id"].as<const char *>());
	writer.EndObject();
	writer.EndObject();

	return response_buffer.GetString();
}

std::string users(Document *json){
	Document tokendata;
	try{
		GetTokenData(&tokendata, json);
	}catch(std::exception& e){
		return simple_error_json(e.what());
	}
	
	pqxx::result res = MyApi::repo->GetUsers();

	StringBuffer response_buffer;
	Writer<StringBuffer> writer(response_buffer);

	writer.StartObject();
	writer.String("result");
	writer.StartArray();

	for(pqxx::result::size_type i = 0; i < res.size(); i++){
		writer.StartObject();
		writer.String("id");
		writer.String(res[i]["id"].as<const char *>());
		writer.String("username");
		writer.String(res[i]["username"].as<const char *>());
		writer.String("email");
		writer.String(res[i]["email"].as<const char *>());
		writer.String("first_name");
		writer.String(res[i]["first_name"].as<const char *>());
		writer.String("last_name");
		writer.String(res[i]["last_name"].as<const char *>());
		writer.EndObject();
	}

	writer.EndArray();
	writer.EndObject();

	return response_buffer.GetString();
}
