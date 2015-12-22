#include <memory>
#include <iostream>
#include <fstream>

#include <atomic>

#include <thread>

#include <set>

#include <boost\asio.hpp>
#include <boost\bind.hpp>
#include <boost\chrono.hpp>

#include "util.hpp"
#include "appd.hpp"
#include "conn.hpp"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/error.h"

using namespace rapidjson;

using namespace boost;

void AppDaemon::connection_acceptor(const system::error_code& ec, Connection* new_connection) {
	if (ec) {
		_log("async_accept error (" + std::to_string(ec.value()) + "): " + ec.message());
		return;
	}

	new_connection->start_receiving();

	Connection* next_connection = new Connection(this, appd_io_service);

	appd_acceptor.async_accept(next_connection->conn_socket,
		bind(&AppDaemon::connection_acceptor, this, asio::placeholders::error, next_connection));
}

AppDaemon::AppDaemon(int port)
	: appd_acceptor(appd_io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
	appd_socket(appd_io_service) {}

void AppDaemon::route(std::string path, std::string(*func)(Document *json), std::vector<std::string> requires){
	AppDaemon::routes[path] = func;
	AppDaemon::required_fields[path] = requires;
}

void AppDaemon::listen(){
	try {
		Connection* next_connection = new Connection(this, appd_io_service);

		appd_acceptor.async_accept(next_connection->conn_socket,
			bind(&AppDaemon::connection_acceptor, this, asio::placeholders::error, next_connection));
	}
	catch (std::exception& e) {
		std::cerr << "appd_acceptor.async_accept exception: " << e.what() << "\n";
	}

	try {
		appd_io_service.run();
	}
	catch (std::exception& e) {
		std::cerr << "appd_io_service.run() exception: " << e.what() << "\n";
	}
}