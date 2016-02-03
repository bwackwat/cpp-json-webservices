#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <stdexcept>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <pqxx/pqxx>

#include "myapi.hpp"
#include "serv.hpp"
#include "pgrepo.hpp"
#include "util.hpp"

using namespace rapidjson;

PostgresRepository* UserService::repo;

std::string root(Document *json){
	return "{\"result\":\"Welcome to the API.\"}";
}

std::string login(Document *json){
	pqxx::result res = UserService::repo->GetUserByLogin((*json)["login"].GetString());
	if(res.size() == 0){
		return simple_error_json("Login does not exist.");
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
	pqxx::result res = UserService::repo->GetUserByUsernameOrEmail(
		(*json)["username"].GetString(),
		(*json)["email"].GetString()
	);
	
	if(res.size() != 0){
		return simple_error_json("Login already exists.");
	}
	
	res = UserService::repo->CreateUser(
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
	std::string tokenjson;
	try{
		tokenjson = decrypt_from_webtoken((*json)["token"].GetString());
	}catch(const std::exception &e){
		return simple_error_json("Invalid token.");
	}
	
	Document tokendata;
	if(tokendata.Parse(tokenjson.c_str()).HasParseError()){
		return simple_error_json("Token parse error.");
	}
	
	if(!tokendata.HasMember("id") ||
	tokendata["id"].GetType() != kNumberType ||
	!tokendata.HasMember("username") ||
	tokendata["username"].GetType() != kStringType ||
	!tokendata.HasMember("proof") ||
	tokendata["proof"].GetType() != kStringType){
		return simple_error_json("Token missing parameter.");
	}
	
	pqxx::result res = UserService::repo->GetUserById(std::to_string(tokendata["id"].GetInt()));
	
	if(res.size() == 0){
		return simple_error_json("Token with invalid username.");
	}
	
	std::string pwd = res[0]["password"].as<const char *>();
	
	if(tokendata["proof"].GetString() != pwd.substr(pwd.length() / 2)){
		return simple_error_json("Token with invalid password.");
	}
	
	res = UserService::repo->GetUsers();

	StringBuffer response_buffer;
	Writer<StringBuffer> writer(response_buffer);

	writer.StartObject();
	writer.String("results");
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

UserService::UserService(int port, std::string name, std::string connection_string)
:WebService(port, name)
{
	repo = new PostgresRepository(connection_string);
	
	init_crypto();
	
	route("/", root);

	route("/user/new", newuser, {
		{"username", kStringType},
		{"password", kStringType},
		{"email", kStringType},
		{"first_name", kStringType},
		{"last_name", kStringType}
	});

	route("/users", users, {
		{"token", kStringType}
	});

	route("/login", login, {
		{"login", kStringType},
		{"password", kStringType}
	});

	listen();
}