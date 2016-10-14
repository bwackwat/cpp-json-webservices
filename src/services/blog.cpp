std::string GetBlogPostsById(std::string id){
	pqxx::result res = MyApi::repo->GetBlogPostsByUserId(id);

	std::stringstream response;
	response << "{\"result\":[";
	for(pqxx::result::size_type i = 0; i < res.size(); i++){
		response << "{\"id\":\"" << res[i]["id"].c_str() << "\",";
		response << "\"title\":" << JsonObject::escape(res[i]["title"].c_str()) << ",";
		response << "\"content\":" << JsonObject::escape(res[i]["content"].c_str()) << ',';
		response << "\"created_on\":\"" << res[i]["created_on"].c_str() << "\"}";
		if(i < res.size() - 1){
			response << ',';
		}
	}
	response << "]}";

	return response.str();
}

std::string GetBlogPostsByUsername(JsonObject* json){
	pqxx::result res = MyApi::repo->GetUserByLogin(json->objectValues["username"]->stringValue);
	if(res.size() == 0){
		return simple_error_json("Username does not exist.");
	}

	return GetBlogPostsById(res[0]["id"].c_str());
}

std::string GetBlogPostsByToken(JsonObject* json){
	JsonObject tokendata;
	try{
		GetTokenData(&tokendata, json);
	}catch(std::exception& e){
		return simple_error_json(e.what());
	}
	
	return GetBlogPostsById(tokendata.objectValues["id"]->stringValue);
}

std::string newblogpost(JsonObject* json){
	JsonObject tokendata;
	try{
		GetTokenData(&tokendata, json);
	}catch(std::exception& e){
		return simple_error_json(e.what());
	}
	
	pqxx::result res = MyApi::repo->CreateBlogPost(
		tokendata.objectValues["id"]->stringValue,
		json->objectValues["title"]->stringValue,
		json->objectValues["content"]->stringValue
	);

	return "{\"result\":\"Successfully submitted the blog post.\"}";
}

std::string putblogpost(JsonObject* json){
	JsonObject tokendata;
	try{
		GetTokenData(&tokendata, json);
	}catch(std::exception& e){
		return simple_error_json(e.what());
	}
	
	pqxx::result res = MyApi::repo->UpdateBlogPost(
		json->objectValues["id"]->stringValue,
		json->objectValues["title"]->stringValue,
		json->objectValues["content"]->stringValue
	);

	return "{\"result\":\"Successfully saved the blog post.\"}";
}