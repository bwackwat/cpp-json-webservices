#include <memory>
#include <iostream>
#include <utility>

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

#include "util.hpp"
#include "conn.hpp"
#include "serv.hpp"
#include "myapi.hpp"
#include "json.hpp"

using namespace boost;

std::string validate_request(char *data, JsonObject *json, std::vector<std::pair<std::string, JsonType>> requires){
	std::stringstream response;

	json->parse(data);

	for (std::vector<int>::size_type i = 0; i != requires.size(); i++){
		if (!json->objectValues.count(requires[i].first.c_str())
		|| json->objectValues[requires[i].first.c_str()]->type != requires[i].second){
			response << "'" << requires[i].first << "' requires a " << JsonObject::typeString[requires[i].second] << ".";

			return simple_error_json(response.str());
		}
	}

	return std::string();
}

Connection::Connection(WebService* service, asio::io_service& io_service)
	: conn_socket(io_service),
	serv_reference(service),
	timeout_timer(io_service) {
}

void Connection::disconnect(const system::error_code& ec){
	if (ec){
		if (ec == asio::error::operation_aborted){
			//Trashed this timerwith cancel();
		}else{
			std::cout << "disconnect async_wait error (" << ec.value() << "): " << ec.message() << std::endl;
		}
	}else{
		std::cout << "DONE: |" << this->conn_socket.remote_endpoint().address().to_string() << std::endl;
		
		conn_socket.shutdown(asio::ip::tcp::socket::shutdown_both);
		conn_socket.close();
		delete this;
	}
}

void Connection::start(){
	// Connection success.
	std::cout << "OPEN: |" << this->conn_socket.remote_endpoint().address().to_string() << std::endl;

	receive();
}

void Connection::receive(){
	timeout_timer.expires_from_now(posix_time::seconds(READ_TIMEOUT));
	timeout_timer.async_wait(bind(&Connection::disconnect, this, asio::placeholders::error));

	conn_socket.async_read_some(asio::buffer(received_data, PACKET_LIMIT), bind(&Connection::received, this, asio::placeholders::error, asio::placeholders::bytes_transferred));
}

void Connection::received(const system::error_code& ec, std::size_t length){
	if (ec) {
		if (ec == asio::error::operation_aborted){
			//Timed out! (Other error codes might need to land here as well/)
		}else{
			std::cout << "received async_read_some error (" << ec.value() << "): " << ec.message() << std::endl;
		}
		return;
	}
	timeout_timer.cancel();

	received_data[length] = '\0';

	char *pstart = strstr(received_data, " ") + 1;
	char *pend = strstr(pstart, " ");

	std::string path(pstart, pend - pstart);
	std::cout << "PATH: |" << path << "|" << std::endl;

	char *received_body = strstr(received_data, "\r\n\r\n") + 4;
	std::cout << "DATA: |" << received_body << "| " << std::strlen(received_data) << " bytes" << std::endl;

	std::string delivery_json;

	if (serv_reference->routes.count(path)){
		auto f = serv_reference->routes[path];

		JsonObject json;

		auto result = validate_request(received_body, &json, serv_reference->required_fields[path]);
		if (!result.empty()){
			delivery_json = result;
		}
		else{
			delivery_json = f(&json);
		}
	}
	else{
		delivery_json = "{\"error\":\"No resource.\"}";
	}

	delivery_data = "HTTP/1.1 200 OK\n";
	delivery_data.append("Accept-Ranges: bytes\n");
	delivery_data.append("Content-Type: application/json\n");
	delivery_data.append("Content-Length: " + std::to_string(delivery_json.length()) + "\n");
	delivery_data.append("\n");
	delivery_data.append(delivery_json);
	
	std::cout << "DELI: |" << delivery_json << "| " << delivery_data.length() << " bytes" << std::endl;

	conn_socket.async_write_some(asio::buffer(delivery_data, delivery_data.length()), bind(&Connection::delivered_done, this, asio::placeholders::error, asio::placeholders::bytes_transferred));
}

void Connection::delivered_done(const system::error_code& ec, std::size_t length){
	if (ec) {
		std::cout << "delivered_done async_write_some error (" << ec.value() << "): " << ec.message() << std::endl;
		disconnect(system::error_code());
		return;
	}

	receive();
}
