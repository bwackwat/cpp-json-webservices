#include <memory>
#include <iostream>
#include <utility>

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

#include "util.hpp"
#include "conn.hpp"
#include "serv.hpp"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/error.h"

using namespace rapidjson;

using namespace boost;

std::string validate_request(char *data, Document *doc, std::vector<std::string> requires){
	std::stringstream response;

	(*doc).Parse(data);
	if ((*doc).HasParseError()){
		int offset = (*doc).GetErrorOffset() - 1;
		response << "Invalid JSON at character " << offset << ": '" << data[offset] << "'.";

		return json_error(response.str());
	}

	for (std::vector<int>::size_type i = 0; i != requires.size(); i++){
		if (!(*doc).HasMember(requires[i].c_str())){
			response << "'" << requires[i] << "' requires a value.";

			return json_error(response.str());
		}
	}

	return std::string();
}

Connection::Connection(WebService* service, asio::io_service& io_service)
	: serv_reference(service),
conn_socket(io_service){
}

void Connection::start_receiving(){
	conn_socket.async_read_some(asio::buffer(received_data, 1024), bind(&Connection::received, this, asio::placeholders::error, asio::placeholders::bytes_transferred));
}

void Connection::received(const system::error_code& ec, std::size_t length) {
	if (ec) {
		_log("received async_read_some error (" + std::to_string(ec.value()) + "): " + ec.message());
		return;
	}
	received_data[length] = '\0';

	char *pstart = strstr(received_data, " ") + 1;
	char *pend = strstr(pstart, " ");

	std::string path(pstart, pend - pstart);
	std::cout << "PATH: |" << path << "|" << std::endl;

	char *received_body = strstr(received_data, "\r\n\r\n") + 4;
	std::cout << "DATA: |" << received_body << "|" << std::endl;

	if (serv_reference->routes.count(path)){
		auto f = serv_reference->routes[path];

		Document doc;

		auto result = validate_request(received_body, &doc, serv_reference->required_fields[path]);
		if (!result.empty()){
			delivery_data = result;
		}
		else{
			delivery_data = f(&doc);
		}
	}
	else{
		delivery_data = "{\"error\":\"No resource.\"}";
	}

	conn_socket.async_write_some(asio::buffer(delivery_data, delivery_data.length()), bind(&Connection::delivered_done, this, asio::placeholders::error, asio::placeholders::bytes_transferred));
}

void Connection::delivered_done(const system::error_code& ec, std::size_t length) {
	if (ec) {
		_log("delivered_done async_write_some error (" + std::to_string(ec.value()) + "): " + ec.message());
		//return;
	}
	conn_socket.shutdown(asio::ip::tcp::socket::shutdown_both);
	conn_socket.close();
	delete this;
}