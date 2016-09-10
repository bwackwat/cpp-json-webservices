std::string GetBlogPostsById(std::string id){
	pqxx::result res = MyApi::repo->GetBlogPostsByUserId(id);

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

std::string GetBlogPostsByUsername(Document* json){
	pqxx::result res = MyApi::repo->GetUserByLogin((*json)["username"].GetString());
	if(res.size() == 0){
		return simple_error_json("Username does not exist.");
	}

	return GetBlogPostsById(res[0]["id"].as<const char *>());
}

std::string GetBlogPostsByToken(Document* json){
	Document tokendata;
	try{
		GetTokenData(&tokendata, json);
	}catch(std::exception& e){
		return simple_error_json(e.what());
	}
	
	return GetBlogPostsById(std::to_string(tokendata["id"].GetInt()));
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
	writer.String("Successfully submitted the blog post.");
	writer.EndObject();

	return response_buffer.GetString();
}

std::string putblogpost(Document* json){
	Document tokendata;
	try{
		GetTokenData(&tokendata, json);
	}catch(std::exception& e){
		return simple_error_json(e.what());
	}
	
	pqxx::result res = MyApi::repo->UpdateBlogPost(
		(*json)["id"].GetString(),
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
