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

PostgresRepository* MyApi::repo;

void GetTokenData(Document* token, Document* json){
	std::string tokenjson;
	try{
		tokenjson = decrypt_from_webtoken((*json)["token"].GetString());
	}catch(const std::exception &e){
		throw std::runtime_error("Invalid token.");
	}
	
	if(token->Parse(tokenjson.c_str()).HasParseError()){
		throw std::runtime_error("Token parse error.");
	}
	
	if(!(*token).HasMember("id") ||
	(*token)["id"].GetType() != kNumberType ||
	!(*token).HasMember("username") ||
	(*token)["username"].GetType() != kStringType ||
	!(*token).HasMember("proof") ||
	(*token)["proof"].GetType() != kStringType){
		throw std::runtime_error("Token missing parameter.");
	}
	
	pqxx::result res = MyApi::repo->GetUserById(std::to_string((*token)["id"].GetInt()));
	
	if(res.size() == 0){
		throw std::runtime_error("Token with invalid username.");
	}
	
	std::string pwd = res[0]["password"].as<const char *>();
	
	if((*token)["proof"].GetString() != pwd.substr(pwd.length() / 2)){
		throw std::runtime_error("Token with invalid password.");
	}
}

std::stringstream routelist;

std::string root(Document *json){
	return pretty("{\"result\":\"Welcome to the API.\",\"routes\":[" + routelist.str() + "]}");
}

#include "services/user.cpp"
#include "services/poi.cpp"
#include "services/blog.cpp"

MyApi::MyApi(int port, std::string name, std::string connection_string, std::string salt)
:WebService(port, name)
{
	repo = new PostgresRepository(connection_string);
	
	init_crypto(salt);
	
	route("/", root);
	route("/users", users, {{"token", kStringType}});
	route("/login", login, {{"username", kStringType},{"password", kStringType}});
	route("/register", newuser, {{"username", kStringType},{"password", kStringType},{"email", kStringType},{"first_name", kStringType},{"last_name", kStringType}});
	
	route("/poi", poi, {{"token", kStringType}});
	route("/user/poi", getuserpoi, {{"token", kStringType}});
	route("/poi/new", newpoi, {{"token", kStringType}, {"label", kStringType}, {"description", kStringType}, {"longitude", kNumberType}, {"latitude", kNumberType}});

	for(auto iter = routes.begin(); iter != routes.end(); ++iter){
		routelist << "{\"" + iter->first + "\":[";
		for(auto &field : required_fields[iter->first]){
			if(&field == &required_fields[iter->first].back()){
				routelist << "\"" << field.first << "\"";
			}else{
				routelist << "\"" << field.first << "\"" << ",";
			}
		}
		if(iter == --routes.end()){
			routelist << "]}";
		}else{
			routelist << "]},";
		}
	}

	listen();
}
