#include <memory>
#include <iostream>
#include <fstream>
#include <atomic>
#include <thread>
#include <set>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/chrono.hpp>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/error.h"

#include "pool.hpp"
#include "util.hpp"
#include "serv.hpp"
#include "conn.hpp"

using namespace rapidjson;
using namespace boost;

void WebService::connection_acceptor(const system::error_code& ec, Connection* new_connection) {
	if (ec) {
		_log("async_accept error (" + std::to_string(ec.value()) + "): " + ec.message());
		return;
	}

	new_connection->start_receiving();

	Connection* next_connection = new Connection(this, service_pool->get_io_service());

	serv_acceptor.async_accept(next_connection->conn_socket,
		bind(&WebService::connection_acceptor, this, asio::placeholders::error, next_connection));
}

WebService::WebService(int port)
	: port(port),
	service_pool(),
	serv_acceptor(service_pool->get_io_service(), asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {}

void WebService::route(std::string path, std::string(*func)(Document *json), std::vector<std::string> requires){
	WebService::routes[path] = func;
	WebService::required_fields[path] = requires;
}

void WebService::listen(){
	try {
		Connection* next_connection = new Connection(this, service_pool->get_io_service());

		serv_acceptor.async_accept(next_connection->conn_socket,
			bind(&WebService::connection_acceptor, this, asio::placeholders::error, next_connection));
	}
	catch (std::exception& e) {
		std::cerr << "appd_acceptor.async_accept exception: " << e.what() << "\n";
	}

	std::cout << "Listening on port " << port << "!" << std::endl;

	try {
		service_pool->run();
	}
	catch (std::exception& e) {
		std::cerr << "appd_io_service.run() exception: " << e.what() << "\n";
	}

	std::cout << "Service on port " << port << " is down!" << std::endl;
}