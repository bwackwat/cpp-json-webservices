std::string poi(Document* json){
	return "{\"result\":\"You're swinging Poi, going to Poi, watching Poi, eating Poi, breeding Poi, and mapping Poi.\"}";
}

std::string getuserpoi(Document* json){
	Document tokendata;
	try{
		GetTokenData(&tokendata, json);
	}catch(std::exception& e){
		return simple_error_json(e.what());
	}
	
	pqxx::result res = MyApi::repo->GetPoiByUserId(std::to_string(tokendata["id"].GetInt()));

	StringBuffer response_buffer;
	Writer<StringBuffer> writer(response_buffer);

	writer.StartObject();
	writer.String("result");
	writer.StartArray();

	for(pqxx::result::size_type i = 0; i < res.size(); i++){
		writer.StartObject();
		writer.String("label");
		writer.String(res[i]["label"].as<const char *>());
		writer.String("description");
		writer.String(res[i]["description"].as<const char *>());
		writer.String("location");
		std::string loc_json = res[i]["location"].as<const char *>();
		writer.RawValue(loc_json.c_str(), loc_json.length(), kObjectType);
		writer.String("created_on");
		writer.String(res[i]["created_on"].as<const char *>());
		writer.EndObject();
	}

	writer.EndArray();
	writer.EndObject();

	return response_buffer.GetString();
}

std::string newpoi(Document* json){
	Document tokendata;
	try{
		GetTokenData(&tokendata, json);
	}catch(std::exception& e){
		return simple_error_json(e.what());
	}
	
	pqxx::result res = MyApi::repo->CreatePoi(
		std::to_string(tokendata["id"].GetInt()),
		(*json)["label"].GetString(),
		(*json)["description"].GetString(),
		(*json)["longitude"].GetDouble(),
		(*json)["latitude"].GetDouble()
	);

	StringBuffer response_buffer;
	Writer<StringBuffer> writer(response_buffer);

	writer.StartObject();
	writer.String("result");
	writer.String("Successfully saved the POI.");
	writer.EndObject();

	return response_buffer.GetString();
}