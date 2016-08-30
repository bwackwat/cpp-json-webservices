#include <memory>
#include <iostream>
#include <utility>
#include <thread>

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "pool.hpp"

using namespace boost;

ServicePool::ServicePool()
: next_io_service(0){
	auto pool_size = std::thread::hardware_concurrency();

	if (pool_size <= 0){
		pool_size = 1;
	}

	std::cout << "Creating " << pool_size << " io_service threads!" << std::endl;

	// Give all the io_services work to do so that their run() functions will not
	// exit until they are explicitly stopped.
	for (std::size_t i = 0; i < pool_size; i++){
		io_service_ptr io_service(new asio::io_service);
		work_ptr work(new asio::io_service::work(*io_service));

		io_services.push_back(io_service);
		workers.push_back(work);
	}
}

void ServicePool::run()
{
	// Create a pool of threads to run all of the io_services.
	std::vector<shared_ptr<thread>> threads;
	for (std::size_t i = 0; i < io_services.size(); ++i)
	{
		shared_ptr<thread> next_thread(new thread(bind(&asio::io_service::run, io_services[i])));
		threads.push_back(next_thread);
	}

	// Wait for all threads in the pool to exit.
	for (std::size_t i = 0; i < threads.size(); ++i)
		threads[i]->join();
}

/*
void io_service_pool::stop()
{
// Explicitly stop all io_services.
for (std::size_t i = 0; i < io_services_.size(); ++i)
io_services_[i]->stop();
}
*/

asio::io_service& ServicePool::get_io_service(){
	//Round robin!
	asio::io_service& io_service = *io_services[next_io_service];
	next_io_service++;
	if (next_io_service == io_services.size()){
		next_io_service = 0;
	}
	return io_service;
}
