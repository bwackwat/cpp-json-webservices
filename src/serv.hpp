#pragma once

#include <memory>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "json.hpp"

using namespace boost;

class Connection;
class ServicePool;

class WebService {
public:
	std::map<std::string, std::string(*)(JsonObject* json)> routes;
	std::map<std::string, std::vector<std::pair<std::string, JsonType>>> required_fields;

	WebService(int port, std::string service_name);
	void route(std::string path, std::string(*func)(JsonObject* json), std::vector<std::pair<std::string, JsonType>> requires = {});
	void listen();
	std::string GetName();
private:
	int port;
	std::string service_name;
	ServicePool* service_pool;
	asio::ip::tcp::acceptor serv_acceptor;

	void connection_acceptor(const system::error_code& ec, Connection* new_connection);
};
