std::string getuserblogposts(Document* json){
	Document tokendata;
	try{
		GetTokenData(&tokendata, json);
	}catch(std::exception& e){
		return simple_error_json(e.what());
	}
	
	pqxx::result res = MyApi::repo->GetBlogPostsByUserId(std::to_string(tokendata["id"].GetInt()));

	StringBuffer response_buffer;
	Writer<StringBuffer> writer(response_buffer);

	writer.StartObject();
	writer.String("result");
	writer.StartArray();

	for(pqxx::result::size_type i = 0; i < res.size(); i++){
		writer.StartObject();
		writer.String("id");
		writer.String(res[i]["id"].as<const char *>());
		writer.String("title");
		writer.String(res[i]["title"].as<const char *>());
		writer.String("content");
		writer.String(res[i]["content"].as<const char *>());
		writer.String("created_on");
		writer.String(res[i]["created_on"].as<const char *>());
		writer.EndObject();
	}

	writer.EndArray();
	writer.EndObject();

	return response_buffer.GetString();
}

std::string newblogpost(Document* json){
	Document tokendata;
	try{
		GetTokenData(&tokendata, json);
	}catch(std::exception& e){
		return simple_error_json(e.what());
	}
	
	pqxx::result res = MyApi::repo->CreateBlogPost(
		std::to_string(tokendata["id"].GetInt()),
		(*json)["title"].GetString(),
		(*json)["content"].GetString()
	);

	StringBuffer response_buffer;
	Writer<StringBuffer> writer(response_buffer);

	writer.StartObject();
	writer.String("result");
	writer.String("Successfully saved the blog post.");
	writer.EndObject();

	return response_buffer.GetString();
}
