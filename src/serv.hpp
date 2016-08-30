#pragma once

#include <memory>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "rapidjson/document.h"

using namespace rapidjson;
using namespace boost;

class Connection;
class ServicePool;

class WebService {
public:
	std::map<std::string, std::string(*)(Document *json)> routes;
	std::map<std::string, std::vector<std::pair<std::string, Type>>> required_fields;

	WebService(int port, std::string service_name);
	void route(std::string path, std::string(*func)(Document *json), std::vector<std::pair<std::string, Type>> requires = {});
	void listen();
	std::string GetName();
private:
	int port;
	std::string service_name;
	ServicePool* service_pool;
	asio::ip::tcp::acceptor serv_acceptor;

	void connection_acceptor(const system::error_code& ec, Connection* new_connection);
};
