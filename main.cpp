#include <iostream>
#include <string>
#include <stdexcept>

#include "myapi.hpp"

int main(int argc, char** argv){
	try{
		UserService api(3000,
			"api.bwackwat.com",
			"dbname=webservice user=postgres password=aq12ws");
	}catch (std::exception& e){
		std::cerr << "UserService exception: " << e.what() << "\n";
		std::getchar();
		return 1;
	}

	return 0;
}