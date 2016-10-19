#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <stdexcept>
#include <iostream>

#include "json.hpp"

//#define DEBUG(msg) std::cout << msg << std::endl;
#define DEBUG(msg)

std::map<enum JsonType, std::string> JsonObject::typeString = {
	{ NOTYPE, "No Json" },
	{ STRING, "String" },
	{ NUMBER, "Number" },
	{ OBJECT, "Object" },
	{ ARRAY, "Array" }
};

const char* JsonObject::parse(const char* str){
	std::stringstream ss;
	enum JsonObjectState objState = NOSTATE;
	JsonObject* value;
	const char* it;
	
	DEBUG("PARSE: " << str)

	for(it = str; *it; ++it){
		DEBUG("LOOP:" << it)
		switch(*it){
		case '\\':
			++it;
			break;
		case '"':
			if(this->type == NOTYPE){
				this->type = STRING;
				DEBUG("START STRING")
				continue;
			}else if(this->type == STRING){
				this->stringValue = ss.str();
				DEBUG("SET STRING")
				return ++it;
			}else if(this->type == OBJECT){
				if(objState == NOSTATE){
					objState = GETKEY;
					continue;
				}else if(objState == GETKEY){
					objState = GOTKEY;
					DEBUG("GOT OBJ KEY: " << ss.str())
					continue;
				}
			}else if(this->type == ARRAY){
				DEBUG(*it << "NEW ARRAY STR")
				value = new JsonObject();
				it = value->parse(it);
				--it;
				arrayValues.push_back(value);
				DEBUG(*it << "GOT STR: " << value->stringify())
				continue;
			}
			break;
		case '{':
			if(this->type == NOTYPE){
				this->type = OBJECT;
				DEBUG("START OBJ")
				continue;
			}else if(this->type == ARRAY){
				DEBUG("PARSE OBJ")
				value = new JsonObject();
				it = value->parse(it);
				arrayValues.push_back(value);
				DEBUG(*it << "GOT OBJ: " << value->stringify())
			}
			break;
		case '}':
			if(this->type == OBJECT){
				if(objState == NOSTATE){
					DEBUG(*it << "ENDOBJECT")
					return it;
				}else{
					DEBUG("EXTRA!!! " << *it)
				}
			}else if(this->type == ARRAY){
				DEBUG("EXPECTED ] ! or else")
				return it;
			}else{
				DEBUG("GOT } !?!?!?!?!!?!?")
			}
			break;
		case '[':
			if(this->type == NOTYPE){
				this->type = ARRAY;
				DEBUG("START ARRAY")
				continue;
			}else if(this->type == STRING){
				this->stringValue = ss.str();
				DEBUG("RESCUE STRING FROM ARRAY START")
				return ++it;
			}else if(this->type == ARRAY){
				value = new JsonObject();
				it = value->parse(it);
				arrayValues.push_back(value);
				DEBUG("GOT ARR: " << value->stringify())
				continue;
			}
			break;
		case ']':
			if(this->type == ARRAY){
				DEBUG("ARRAYEND")
				return it;
			}else if(this->type == STRING){
				this->stringValue = ss.str();
				DEBUG("RESCUE STRING FROM ARRAY END")
				return it;
			}else if(this->type == OBJECT){
				if(objState != GETKEY &&
				objState != GETVALUE){
					DEBUG("RESCUE OBJECT FROM ARRAY END")
					return it;
				}
			}
			break;
		case ':':
			if(this->type == OBJECT){
				if(objState == GOTKEY){
					DEBUG("GETTING VAL!")
					value = new JsonObject();
					it = value->parse(it);
					if(value->type != OBJECT && value->type != ARRAY){
						DEBUG("IS A | " << typeString[value->type])
						--it;
					}
					objectValues[ss.str()] = value;
					DEBUG("GOT VAL: " << value->stringify() << " FOR " << ss.str())
					ss.str(std::string());
					objState = NOSTATE;
					DEBUG("... " << it)
				}
			}
			break;
		}
		if(this->type == STRING){
			ss << *it;
			continue;
		}else if(this->type == OBJECT){
			if(objState == GETKEY){
				ss << *it;
				continue;
			}
		}
	}
	return --it;
}

std::string JsonObject::escape(std::string value){
	std::stringstream escaped;
	escaped << '"';
	for(int i = 0; i < value.length(); ++i){
		if(value[i] == '"' ||
		value[i] == '\\'){
			escaped << '\\';
		}
		escaped << value[i];
	}
	escaped << '"';
	return escaped.str();
}
		
std::string JsonObject::stringify(bool pretty, int depth){
	std::stringstream ss;
	switch(this->type){
		case STRING:
			return escape(this->stringValue);
		case NUMBER:
			return std::to_string(this->numberValue);
		case OBJECT:
			ss << '{';
			if(pretty){
				ss << '\n';
				depth++;
			}
			for(auto it = this->objectValues.begin(); it != this->objectValues.end(); ++it){
				if(pretty){
					ss << std::string(depth, '\t');
				}
				ss << escape(it->first) << ':';
				if(it == --this->objectValues.end()){
                                        ss << it->second->stringify(pretty, depth);
                                }else{
                                        ss << it->second->stringify(pretty, depth) << ',';
                                }
				if(pretty){
					ss << '\n';
				}
			}
			if(pretty){
				depth--;
				ss << std::string(depth, '\t');
			}
			ss << '}';
			return ss.str();
		case ARRAY:
			ss << '[';
			if(pretty){
                                ss << '\n';
                                depth++;
                        }
			for(JsonObject* item : this->arrayValues){
				if(pretty){
					ss << std::string(depth, '\t');
				}
				if(item == this->arrayValues.back()){
					ss << item->stringify(pretty, depth);
				}else{
					ss << item->stringify(pretty, depth) << ',';
				}
				if(pretty){
					ss << '\n';
				}
			}
			if(pretty){
                                depth--;
                                ss << std::string(depth, '\t');
                        }
			ss << ']';
			return ss.str();
		default:
			return "";
	}
}

JsonObject::~JsonObject(){
	if(this->type == OBJECT){
		for(auto it = this->objectValues.begin(); it != this->objectValues.end(); ++it){
			delete it->second;
		}
	}else if(this->type == ARRAY){
		for(JsonObject* item : this->arrayValues){
			delete item;
		}
	}
}
