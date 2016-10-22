#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <stdexcept>

#include <pqxx/pqxx>

#include "myapi.hpp"
#include "serv.hpp"
#include "pgrepo.hpp"
#include "util.hpp"
#include "json.hpp"

PostgresRepository* MyApi::repo;

void GetTokenData(JsonObject* token, JsonObject* json){
	std::string tokenjson;
	try{
		tokenjson = decrypt_from_webtoken(json->objectValues["token"]->stringValue);
	}catch(const std::exception &e){
		throw std::runtime_error("Invalid token.");
	}
	
	token->parse(tokenjson.c_str());

	if(!(token->objectValues.count("id") &&
	token->objectValues["id"]->type == STRING &&
	token->objectValues.count("username") &&
	token->objectValues["username"]->type == STRING &&
	token->objectValues.count("proof") &&
	token->objectValues["proof"]->type == STRING)){
		throw std::runtime_error("Token missing parameter.");
	}
	
	pqxx::result res = MyApi::repo->GetUserById(token->objectValues["id"]->stringValue);
	
	if(res.size() == 0){
		throw std::runtime_error("Token with invalid username.");
	}
	
	std::string pwd = res[0]["password"].c_str();
	
	if(token->objectValues["proof"]->stringValue != pwd.substr(pwd.length() / 2)){
		throw std::runtime_error("Token with invalid password.");
	}
}

JsonObject routes_object;

std::string root(JsonObject *json){
	return routes_object.stringify(true);
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
	route("/token", tokencheck, {{"token", STRING}});
	route("/user", users, {{"token", STRING}});
	route("/login", login, {{"username", STRING},{"password", STRING}});
	route("/user/new", newuser, {{"username", STRING},{"password", STRING},{"email", STRING},{"first_name", STRING},{"last_name", STRING}});
	
	route("/poi", poi, {{"token", STRING}});
	route("/user/poi", getuserpoi, {{"token", STRING}});
	route("/poi/new", newpoi, {{"token", STRING}, {"label", STRING}, {"description", STRING}, {"longitude", STRING}, {"latitude", STRING}});

	route("/blog", GetBlogPostsByUsername, {{"username", STRING}});
	route("/user/blog", GetBlogPostsByToken, {{"token", STRING}});
	route("/blog/new", newblogpost, {{"token", STRING}, {"title", STRING}, {"content", STRING}});
	route("/blog/put", putblogpost, {{"token", STRING}, {"id", STRING}, {"title", STRING}, {"content", STRING}});

	routes_object.type = OBJECT;
	routes_object.objectValues["result"] = new JsonObject();
	routes_object.objectValues["result"]->type = STRING;
	routes_object.objectValues["result"]->stringValue = "Welcome to the API. This is a list of the routes and their required JSON parameters";
	for(auto iter = routes.begin(); iter != routes.end(); ++iter){
		routes_object.objectValues[iter->first] = new JsonObject();
		routes_object.objectValues[iter->first]->type = ARRAY;
		for(auto &field : required_fields[iter->first]){
			JsonObject* array_item = new JsonObject();
			array_item->type = STRING;
			array_item->stringValue = field.first;
			routes_object.objectValues[iter->first]->arrayValues.push_back(array_item);
		}
	}

	listen();
}
