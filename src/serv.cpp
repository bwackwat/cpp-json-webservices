#include <memory>
#include <iostream>
#include <thread>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/chrono.hpp>

#include "pool.hpp"
#include "util.hpp"
#include "serv.hpp"
#include "conn.hpp"
#include "json.hpp"

using namespace boost;

void WebService::connection_acceptor(const system::error_code& ec, Connection* new_connection) {
	if (ec) {		
		std::cout << "connection_acceptor async_accept error (" << ec.value() << "): " << ec.message() << std::endl;
		return;
	}

	new_connection->start();

	Connection* next_connection = new Connection(this, service_pool->get_io_service());

	serv_acceptor.async_accept(next_connection->conn_socket,
		bind(&WebService::connection_acceptor, this, asio::placeholders::error, next_connection));
}

WebService::WebService(int port, std::string service_name)
	: port(port),
	service_name(service_name),
	service_pool(new ServicePool()),
	serv_acceptor(service_pool->get_io_service(), asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {}

void WebService::route(std::string path, std::string(*func)(JsonObject *json), std::vector<std::pair<std::string, JsonType>> requires){
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

	std::cout << "Listening on port " << port << " with PID " << ::getpid() << "." << std::endl;

	try {
		service_pool->run();
	}
	catch (std::exception& e) {
		std::cerr << "appd_io_service.run() exception: " << e.what() << "\n";
	}

	std::cout << "Service on port " << port << " is down!" << std::endl;
}

std::string WebService::GetName(){
	return service_name;
}