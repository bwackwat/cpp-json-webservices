#include <iostream>
#include <string>
#include <stdexcept>

#include <redox.hpp>
using namespace redox;

#include "myapi.hpp"

int main(int argc, char** argv){
	Redox rdx;
	if(!rdx.connect("localhost", 6379)){
		std::cerr << "No localhost redis for configuration." << std::endl;
		return 1;		
	}
	
	int port = std::stoi(rdx.get("WEBSERVICE_PORT"));
	std::string host = rdx.get("WEBSERVICE_HOST");
	std::string conn = rdx.get("POSTGRES_MASTER_CONNECTION_STRING");
	
	rdx.disconnect();
	
	try{
		MyApi api(port,	host, conn);
	}catch (std::exception& e){
		std::cerr << "UserService exception: " << e.what() << "\n";
		std::getchar();
		return 1;
	}

	return 0;
}