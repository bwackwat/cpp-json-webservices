#pragma once

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

#include "../myapi.hpp"
#include "../serv.hpp"
#include "../pgrepo.hpp"
#include "../util.hpp"
#include "userservices.hpp"

using namespace rapidjson;

PostgresRepository* UserService::repo;

std::string root(Document *json);
std::string login(Document *json);
std::string newuser(Document *json);
std::string users(Document *json);