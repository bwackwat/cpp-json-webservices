#pragma once

#include <boost/asio.hpp>

using namespace boost;

class WebService;

class Connection {
public:
	asio::ip::tcp::socket conn_socket;

	void start_receiving();
	Connection(WebService* service, asio::io_service& io_service);
private:
	WebService* serv_reference;

	char received_data[8192];
	std::string delivery_data;

	void received(const system::error_code& ec, std::size_t length);
	void delivered_done(const system::error_code& ec, std::size_t length);
};