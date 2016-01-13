#include <string>
#include <iostream>
#include <thread>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

void _log(std::string logdata){
	std::cout << std::this_thread::get_id() << ": " << logdata << std::endl;
}

std::string json_error(std::string msg){
	/* ALTERNATIVE:

	StringBuffer s;
	Writer<StringBuffer> writer(s);

	writer.StartObject();
	writer.String("error");
	writer.String(msg);
	writer.EndObject();

	return s.GetString();

	*/

	Document response_doc;
	StringBuffer response_doc_buffer;
	//response_doc_buffer.Clear();
	Writer<StringBuffer> writer(response_doc_buffer);

	response_doc.SetObject();
	Value res(msg.c_str(), response_doc.GetAllocator());
	response_doc.AddMember("error", res, response_doc.GetAllocator());
	response_doc.Accept(writer);

	return response_doc_buffer.GetString();
}