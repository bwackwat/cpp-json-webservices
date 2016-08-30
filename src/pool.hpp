#pragma once

#include <memory>
#include <iostream>
#include <utility>

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

using namespace boost;

class ServicePool {
public:
	ServicePool();

	void run();
	asio::io_service& get_io_service();

private:
	typedef shared_ptr<asio::io_service> io_service_ptr;
	typedef shared_ptr<asio::io_service::work> work_ptr;

	std::vector<io_service_ptr> io_services;
	std::vector<work_ptr> workers;
	std::size_t next_io_service;
};
