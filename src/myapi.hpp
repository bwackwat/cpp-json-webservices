#pragma once

#include "serv.hpp"

class WebService;
class PostgresRepository;

class MyApi : public WebService {
public:
	static PostgresRepository* repo;
	MyApi(int port, std::string name, std::string connection_string, std::string salt);
};
