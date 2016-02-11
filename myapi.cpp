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

#include <redox.hpp>

#include "myapi.hpp"
#include "serv.hpp"
#include "pgrepo.hpp"
#include "util.hpp"

using namespace rapidjson;

PostgresRepository* MyApi::repo;

#include "services/userservices.cpp"

MyApi::MyApi(int port, std::string name, std::string connection_string)
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