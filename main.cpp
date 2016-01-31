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

const std::string POSTGRESQL_CONNSTRING = "dbname=webservice user=postgres password=aq12ws";

std::string root(Document *json){
	return "{\"result\":\"Welcome to the API.\"}";
}

std::string newuser(Document *json){
	pqxx::connection conn(POSTGRESQL_CONNSTRING);
	pqxx::work txn(conn);
	pqxx::result res;	

	try{
		res = txn.exec(
			"INSERT INTO users"
			"(id, username, password, email, first_name, last_name) "
			"VALUES (DEFAULT, " +
			txn.quote((*json)["username"].GetString()) + ", " +
			txn.quote((*json)["password"].GetString()) + ", " +
			txn.quote((*json)["email"].GetString()) + ", " +
			txn.quote((*json)["first_name"].GetString()) + ", " +
			txn.quote((*json)["last_name"].GetString()) +
			") RETURNING id;"
		);
	}catch(const pqxx::pqxx_exception &e){
		std::string error = e.base().what();
		auto start = error.find("DETAIL") + 9;
		auto end = error.find("\n", start);
		return "{\"error\":\"" + error.substr(start, end - start) + "\"}";
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
		api.route("/user/new", newuser,
			{ "username", "password", "email", "first_name", "last_name" });
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
