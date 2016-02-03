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

const std::string POSTGRESQL_CONNSTRING = "dbname=webservice user=postgres password=aq12ws";
PostgresRepository* UserService::repo;

std::string root(Document *json){
	return "{\"result\":\"Welcome to the API.\"}";
}

std::string login(Document *json){
	pqxx::result res = UserService::repo->GetUserByLogin((*json)["login"].GetString());
	
	pqxx::connection conn(POSTGRESQL_CONNSTRING);
	pqxx::work txn(conn);
	res = txn.exec(
			"SELECT * FROM users WHERE username = " +
	txn.quote((*json)["username"].GetString()) + ";"
	);

	if(res.size() == 0){
		return simple_error_json("Username does not exist.");
	}

	std::string given_hash = hash_password((*json)["password"].GetString());
	std::cout << "HASPWED:" << given_hash << std::endl;

	if(given_hash == res[0]["password"].as<const char *>()){
		std::cout << "CORRECT PASSWORD!" << std::endl;

		StringBuffer token_buffer;
		Writer<StringBuffer> token_writer(token_buffer);

		token_writer.StartObject();
		token_writer.String("id");
		token_writer.Int(res[0]["id"].as<int>());
		token_writer.String("username");
		token_writer.String(res[0]["username"].as<const char *>());
		token_writer.String("proof");
		token_writer.String(given_hash.substr(given_hash.length() / 2).c_str());
		token_writer.EndObject();

		std::string tokendata = token_buffer.GetString();

		std::cout << "UGITOKEN:" << tokendata << std::endl;

		StringBuffer response_buffer;
		Writer<StringBuffer> writer(response_buffer);

		writer.StartObject();
		writer.String("result");
		writer.StartObject();
		writer.String("token");
		writer.String(encrypt_to_webtoken(tokendata).c_str());
		writer.EndObject();
		writer.EndObject();

		return response_buffer.GetString();
	}else{
		return simple_error_json("Incorrect password.");
	}

//Test

/*	std::string webtoken = encrypt_to_webtoken("{\"username\":\"bwackwat\",\"proof\":\"LOADEDSHITSTORM\"}");
	std::cout << "TOKEN:" << webtoken << ":ENDTOKEN" << std::endl;
	std::string data_to_check = decrypt_from_webtoken(webtoken);
	std::cout << "VALUE:" << data_to_check << ":ENDVALUE" << std::endl;*/

}	

std::string newuser(Document *json){	
	pqxx::connection conn(POSTGRESQL_CONNSTRING);
	pqxx::work txn(conn);
	pqxx::result res;

	auto hashed_password = hash_password((*json)["password"].GetString());
	try{
		res = txn.exec(
			"INSERT INTO users"
			"(id, username, password, email, first_name, last_name) "
			"VALUES (DEFAULT, " +
			txn.quote((*json)["username"].GetString()) + ", " +
			txn.quote(hashed_password) + ", " +
			txn.quote((*json)["email"].GetString()) + ", " +
			txn.quote((*json)["first_name"].GetString()) + ", " +
			txn.quote((*json)["last_name"].GetString()) +
			") RETURNING id;"
		);
	}catch(const pqxx::pqxx_exception &e){
		std::string error = e.base().what();
		auto start = error.find("DETAIL") + 9;
		auto end = error.find("\n", start);
		return simple_error_json(error.substr(start, end - start));
	}
	txn.commit();

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
	pqxx::connection conn(POSTGRESQL_CONNSTRING);
	pqxx::work txn(conn);
	pqxx::result res;

	try{
		std::string tokenjson = decrypt_from_webtoken((*json)["token"].GetString());
		std::cout << "JSON:" << tokenjson << ":END" << std::endl;

		Document tokendata;
		if(tokendata.Parse(tokenjson.c_str()).HasParseError()){
			throw std::runtime_error("Token parse error.");
		}

		//MAKE SURE TOKEN HAS THE FIELDS THEY ARE MADE WITH
		if(!tokendata.HasMember("id") ||
		tokendata["id"].GetType() != kNumberType ||
		!tokendata.HasMember("username") ||
		tokendata["username"].GetType() != kStringType ||
		!tokendata.HasMember("proof") ||
		tokendata["proof"].GetType() != kStringType){
			throw std::runtime_error("Token missing parameter.");
		}

		res = txn.exec(
			"SELECT * FROM users WHERE id = " +
			txn.quote(std::to_string(tokendata["id"].GetInt())) + ";"
		);
		if(res.size() == 0){
			throw std::runtime_error("Token with invalid username.");
		}
		std::string pwd = res[0]["password"].as<const char *>();
		if(tokendata["proof"].GetString() != pwd.substr(pwd.length() / 2)){
			throw std::runtime_error("Token with invalid password.");
		}
	}catch(const std::exception &e){
		std::cout << e.what() << std::endl;
		return simple_error_json("Invalid token.");
	}

	res = txn.exec(
		"SELECT * FROM users;"
	);

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