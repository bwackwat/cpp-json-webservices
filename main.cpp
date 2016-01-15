#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <pqxx/pqxx>

#include "serv.hpp"

using namespace rapidjson;

std::string root(Document *json){
	return "Welcome to the API.";
}

std::string newuser(Document *json){
	std::stringstream response;
	response << "User:" << (*json)["username"].GetString() << "/" << (*json)["password"].GetString();
	return response.str();
}

std::string users(Document *json){
	pqxx::connection conn("dbname=webservice user=postgres");
	pqxx::work txn(conn);
	pqxx::result res = txn.exec(
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

int main(int argc, char** argv){
	try{
		WebService api(3000);

		api.route("/", root);
		api.route("/user/new", newuser, { "username", "password" });
		api.route("/users", users);

		api.listen();
	}
	catch (std::exception& e){
		std::cerr << "Global exception: " << e.what() << "\n";
		std::getchar();
		return 1;
	}

	return 0;
}
