#include <stdlib.h>

#include <iostream>
#include <string>
#include <stdexcept>
#include <fstream>

#include "myapi.hpp"
#include "json.hpp"

int main(int argc, char** argv){
	int port = 8000;
	std::string hostname = "webservice.com";
	std::string connection_string = "server=localhost; db=webservice";
	std::string argon2_salt = "makesuretouse16!";
	JsonObject config_json;
	std::ifstream config_file("./bin/configuration.json");
	std::string config_data((std::istreambuf_iterator<char>(config_file)),
		(std::istreambuf_iterator<char>()));
		
	config_json.parse(config_data.c_str());

	std::cout << "Loaded: ./bin/configuration.json" << std::endl;
	std::cout << config_json.stringify(true) << std::endl;
	
	if(config_json.objectValues.count("port") &&
	config_json.objectValues["port"]->type == STRING){
		port = std::atoi(config_json.objectValues["port"]->stringValue.c_str());
	}
	std::cout << "port: " << port << std::endl;

	if(config_json.objectValues.count("hostname") &&
	config_json.objectValues["hostname"]->type == STRING){
		hostname = config_json.objectValues["hostname"]->stringValue;
	}
	std::cout << "hostname: " << hostname << std::endl;

	if(config_json.objectValues.count("connection_string") &&
	config_json.objectValues["connection_string"]->type == STRING){
		connection_string = config_json.objectValues["connection_string"]->stringValue;
	}
	std::cout << "connection_string: " << connection_string << std::endl;

	if(config_json.objectValues.count("argon2_salt") &&
	config_json.objectValues["argon2_salt"]->type == STRING){
		argon2_salt = config_json.objectValues["argon2_salt"]->stringValue;
	}
	std::cout << "argon2_salt: " << argon2_salt << std::endl;

	try{
		MyApi api(port,	hostname, connection_string, argon2_salt);
	}catch (std::exception& e){
		std::cerr << "MyApi exception: " << e.what() << "\n";
		std::getchar();
	}

	// Shouldn't end.
	return 1;
}