#include <iostream>
#include <string>
#include <stdexcept>

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

using namespace rapidjson;

#include "myapi.hpp"

int main(int argc, char** argv){
	int port = 8000;
	std::string host = "www.webservice.com";
	std::string postgres_connection_string = "server=localhost; db=webservice";
	std::string argon2_salt = "makesuretouse16!";
	char buffer[65536];
	FILE* configurationFile = fopen("bin/configuration.json", "rb");
	if(configurationFile == 0){
		std::cout << argv[0] << ": Configuration file error: " << errno << "\n";
		return 1;
	}
	FileReadStream frs(configurationFile, buffer, sizeof(buffer));
	Document config;
	
	config.ParseStream<0, UTF8<>, FileReadStream>(frs);

	if(config.HasParseError()){
		int offset = config.GetErrorOffset();
		std::cerr << "Invalid configuration JSON at character " << offset << ": '" << buffer[offset] << ".\n";
		return 1;
	}else{
		std::cout << "Using values from configuration.json:\n";
	}

	if(config.HasMember("port") && config["port"].GetType() == kNumberType){
		port = config["port"].GetInt();
		std::cout << "port: " << port << "\n";
	}

	if(config.HasMember("host") && config["host"].GetType() == kStringType){
		host = config["host"].GetString();
		std::cout << "host: " << host << "\n";
	}

	if(config.HasMember("postgres_connection_string")
	&& config["postgres_connection_string"].GetType() == kStringType){
		postgres_connection_string = config["postgres_connection_string"].GetString();
		std::cout << "postgres_connection_string: " << postgres_connection_string << "\n";
	}

	if(config.HasMember("argon2_salt") && config["argon2_salt"].GetType() == kStringType){
		argon2_salt = config["argon2_salt"].GetString();
		std::cout << "argon2_salt: " << argon2_salt << "\n";
	}

	try{
		MyApi api(port,	host, postgres_connection_string, argon2_salt);
	}catch (std::exception& e){
		std::cerr << "MyApi exception: " << e.what() << "\n";
		std::getchar();
		return 1;
	}

	return 0;
}
