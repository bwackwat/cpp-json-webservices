std::string poi(JsonObject* json){
	return "{\"result\":\"You're swinging Poi, going to Poi, watching Poi, eating Poi, breeding Poi, and mapping Poi.\"}";
}

std::string getuserpoi(JsonObject* json){
	JsonObject tokendata;
	try{
		GetTokenData(&tokendata, json);
	}catch(std::exception& e){
		return simple_error_json(e.what());
	}
	
	pqxx::result res = MyApi::repo->GetPoiByUserId(tokendata.objectValues["id"]->stringValue);

	std::stringstream response;
	response << "{\"result\":[";
	for(pqxx::result::size_type i = 0; i < res.size(); i++){
		response << "{\"label\":\"" << res[i]["label"].c_str() << "\",";
		response << "\"description\":\"" << res[i]["description"].c_str() << "\",";
		response << "\"location\":\"" << res[i]["location"].c_str() << "\",";
		response << "\"created_on\":\"" << res[i]["created_on"].c_str() << "\"}";
		if(i < res.size() - 1){
			response << ',';
		}
	}
	response << "}";

	return response.str();
}

std::string newpoi(JsonObject* json){
	JsonObject tokendata;
	try{
		GetTokenData(&tokendata, json);
	}catch(std::exception& e){
		return simple_error_json(e.what());
	}
	
	pqxx::result res = MyApi::repo->CreatePoi(
		tokendata.objectValues["id"]->stringValue,
		json->objectValues["label"]->stringValue,
		json->objectValues["description"]->stringValue,
		std::stod(json->objectValues["longitude"]->stringValue),
		std::stod(json->objectValues["latitude"]->stringValue)
	);

	return "{\"result\":\"Successfully saved the POI.\"}";
}