#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>

#include "rapidjson/document.h"

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

int main(int argc, char** argv){
	try{
		WebService api(3000);

		api.route("/", root);
		api.route("/user/new", newuser, { "username", "password" });

		api.listen();
	}
	catch (std::exception& e){
		std::cerr << "Global exception: " << e.what() << "\n";
		std::getchar();
		return 1;
	}

	return 0;
}
