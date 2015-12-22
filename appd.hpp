#ifndef APPD_SERVER
#define APPD_SERVER

#include <memory>
#include <iostream>
#include <fstream>

#include <atomic>

#include <thread>

#include <set>

#include <boost\asio.hpp>
#include <boost\bind.hpp>

#include "conn.hpp"

#include "rapidjson/document.h"

using namespace rapidjson;

using namespace boost;

class AppDaemon {
private:
	asio::io_service appd_io_service;
	asio::ip::tcp::acceptor appd_acceptor;
	asio::ip::tcp::socket appd_socket;

	void connection_acceptor(const system::error_code& ec, Connection* new_connection);
public:
	std::map<std::string, std::string(*)(Document *json)> routes;
	std::map<std::string, std::vector<std::string>> required_fields;

	AppDaemon(int port);
	void route(std::string path, std::string(*func)(Document *json), std::vector<std::string> requires = {});
	void listen();
};

#endif