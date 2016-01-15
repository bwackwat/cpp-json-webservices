#pragma once

#include <memory>
#include <iostream>
#include <fstream>
#include <atomic>
#include <thread>
#include <set>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "rapidjson/document.h"

using namespace rapidjson;
using namespace boost;

class Connection;
class ServicePool;

class WebService {
private:
	ServicePool* service_pool;
	asio::ip::tcp::acceptor serv_acceptor;

	void connection_acceptor(const system::error_code& ec, Connection* new_connection);
public:
	int port;

	std::map<std::string, std::string(*)(Document *json)> routes;
	std::map<std::string, std::vector<std::string>> required_fields;

	WebService(int port);
	void route(std::string path, std::string(*func)(Document *json), std::vector<std::string> requires = {});
	void listen();
};