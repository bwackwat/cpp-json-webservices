#pragma once

#include <memory>
#include <iostream>
#include <fstream>
#include <atomic>
#include <thread>
#include <set>

#include "rapidjson/document.h"

#include "serv.hpp"

using namespace rapidjson;

class WebService;
class PostgresRepository;

class UserService : public WebService {
public:
	static PostgresRepository* repo;
	UserService(int port, std::string name, std::string connection_string);
};