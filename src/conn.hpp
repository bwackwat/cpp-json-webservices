#pragma once

#include <boost/asio.hpp>

#define PACKET_LIMIT 8192 // 8 megabytes
#define READ_TIMEOUT 10 // 10 seconds before reconnection.

using namespace boost;

class WebService;

class Connection {
public:
	asio::ip::tcp::socket conn_socket;

	void start();
	Connection(WebService* service, asio::io_service& io_service);
private:
	WebService* serv_reference;
	asio::deadline_timer timeout_timer;

	char received_data[PACKET_LIMIT];
	std::string delivery_data;

	void disconnect(const system::error_code& ec);
	void receive();
	void received(const system::error_code& ec, std::size_t length);
	void delivered_done(const system::error_code& ec, std::size_t length);
};