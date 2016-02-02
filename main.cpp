#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <stdexcept>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <pqxx/pqxx>

#include "argon2.h"

#include "cryptopp/aes.h"
#include "cryptopp/osrng.h"
#include "cryptopp/modes.h"
#include "cryptopp/base64.h"
#include "cryptopp/filters.h"
#include "cryptopp/cryptlib.h"

#include "serv.hpp"

using namespace rapidjson;

const std::string POSTGRESQL_CONNSTRING = "dbname=webservice user=postgres password=aq12ws";
byte* token_key = new byte[CryptoPP::AES::MAX_KEYLENGTH];
byte* token_iv = new byte[CryptoPP::AES::BLOCKSIZE];

std::string root(Document *json){
	return "{\"result\":\"Welcome to the API.\"}";
}

std::string hash_password(std::string password){
	const uint32_t t_cost = 5;
	const uint32_t m_cost = 1 << 16; //about 65MB
	const uint32_t parallelism = 1; //can use std::thread::hardware_concurrency();

	std::vector<uint8_t> pwdvec(password.begin(), password.end());
	uint8_t* pwd = &pwdvec[0];
	const size_t pwdlen = password.length();

	std::string nonce = "itmyepicsalt!@12";
	std::vector<uint8_t> saltvec(nonce.begin(), nonce.end());
	uint8_t* salt = &saltvec[0];
	const size_t saltlen = nonce.length();

	size_t hashlen = 128;
	uint8_t* hash = new uint8_t[hashlen];
	
	size_t encodedlen = hashlen * 2;
	char* encoded = new char[encodedlen];

	argon2_type type = Argon2_d;

	int res = argon2_hash(t_cost, m_cost, parallelism,
		pwd, pwdlen, salt, saltlen,
		hash, hashlen, encoded, encodedlen, type);
	
	if(res){
		std::string hashing_error = std::string("BrokenHashedPwd:");
		hashing_error += res;
		return hashing_error;
	}else{
		return std::string(encoded);
	}
}

std::string encrypt_to_webtoken(std::string data){
	std::string token;
	CryptoPP::StringSink* sink = new CryptoPP::StringSink(token);
	CryptoPP::Base64Encoder* base64_enc = new CryptoPP::Base64Encoder(sink, false);
	CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption enc(token_key, CryptoPP::AES::MAX_KEYLENGTH, token_iv);
	CryptoPP::StreamTransformationFilter* aes_enc = new CryptoPP::StreamTransformationFilter(enc, base64_enc);
	CryptoPP::StringSource enc_source(data, true, aes_enc);
	return token;
}

std::string decrypt_from_webtoken(std::string token){
	std::string data;
	CryptoPP::StringSink* sink = new CryptoPP::StringSink(data);
	CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption dec(token_key, CryptoPP::AES::MAX_KEYLENGTH, token_iv);
	CryptoPP::StreamTransformationFilter* aes_dec = new CryptoPP::StreamTransformationFilter(dec, sink);
	CryptoPP::Base64Decoder* base64_dec = new CryptoPP::Base64Decoder(aes_dec);
	CryptoPP::StringSource dec_source(token, true, base64_dec);
	return data;
}

std::string simple_error_json(std::string message){
	return "{\"error\":\"" + message + "\"}";
}

std::string login(Document *json){
	pqxx::connection conn(POSTGRESQL_CONNSTRING);
        pqxx::work txn(conn);
        pqxx::result res = txn.exec(
                "SELECT * FROM users WHERE username = " +
		txn.quote((*json)["username"].GetString()) + ";"
        );

	if(res.size() == 0){
		return simple_error_json("Username does not exist.");
	}

	std::string given_hash = hash_password((*json)["password"].GetString());
	std::cout << "HASPWED:" << given_hash << std::endl;

	if(given_hash == res[0]["password"].as<const char *>()){
		std::cout << "CORRECT PASSWORD!" << std::endl;

		StringBuffer response_buffer;
		Writer<StringBuffer> writer(response_buffer);

		writer.StartObject();
		writer.String("id");
		writer.Int(res[0]["id"].as<int>());
		writer.String("username");
		writer.String(res[0]["username"].as<const char *>());
		writer.String("proof");
		writer.String(given_hash.substr(given_hash.length() / 2).c_str());
		writer.EndObject();

		std::string tokendata = response_buffer.GetString();

		std::cout << "UGITOKEN:" << tokendata << std::endl;

		response_buffer.Clear();
		writer.Reset(response_buffer);

		writer.StartObject();
		writer.String("result");
		writer.StartObject();
		writer.String("token");
		writer.String(encrypt_to_webtoken(tokendata).c_str());
		writer.EndObject();
		writer.EndObject();

		return response_buffer.GetString();
	}else{
		return simple_error_json("Incorrect password.");
	}

//Test

/*	std::string webtoken = encrypt_to_webtoken("{\"username\":\"bwackwat\",\"proof\":\"LOADEDSHITSTORM\"}");
	std::cout << "TOKEN:" << webtoken << ":ENDTOKEN" << std::endl;
	std::string data_to_check = decrypt_from_webtoken(webtoken);
	std::cout << "VALUE:" << data_to_check << ":ENDVALUE" << std::endl;*/

}	

std::string newuser(Document *json){
	pqxx::connection conn(POSTGRESQL_CONNSTRING);
	pqxx::work txn(conn);
	pqxx::result res;	

	auto hashed_password = hash_password((*json)["password"].GetString());
	try{
		res = txn.exec(
			"INSERT INTO users"
			"(id, username, password, email, first_name, last_name) "
			"VALUES (DEFAULT, " +
			txn.quote((*json)["username"].GetString()) + ", " +
			txn.quote(hashed_password) + ", " +
			txn.quote((*json)["email"].GetString()) + ", " +
			txn.quote((*json)["first_name"].GetString()) + ", " +
			txn.quote((*json)["last_name"].GetString()) +
			") RETURNING id;"
		);
	}catch(const pqxx::pqxx_exception &e){
		std::string error = e.base().what();
		auto start = error.find("DETAIL") + 9;
		auto end = error.find("\n", start);
		return simple_error_json(error.substr(start, end - start));
	}
	txn.commit();

	StringBuffer response_buffer;
	Writer<StringBuffer> writer(response_buffer);

	writer.StartObject();
	writer.String("result");
	writer.StartObject();
	writer.String("id");
	writer.String(res[0]["id"].as<const char *>());
	writer.EndObject();
	writer.EndObject();

	return response_buffer.GetString();
}

std::string users(Document *json){
	pqxx::connection conn(POSTGRESQL_CONNSTRING);
	pqxx::work txn(conn);
	pqxx::result res;

	try{
		Document tokendata;
		tokendata.Parse(decrypt_from_webtoken((*json)["token"].GetString()).c_str());
		res = txn.exec(
			"SELECT * FROM users WHERE id = " +
			std::to_string(tokendata["id"].GetInt()) + ";"
		);
		if(res.size() == 0){
			throw std::runtime_error("Token with invalid username.");
		}
		std::string pwd = res[0]["password"].as<const char *>();
		if(tokendata["proof"].GetString() != pwd.substr(pwd.length() / 2)){
			throw std::runtime_error("Token with invalid password.");
		}
	}catch(const std::exception &e){
		std::cout << e.what() << std::endl;
		return simple_error_json("Invalid token.");
	}

	res = txn.exec(
		"SELECT * FROM users;"
	);

	StringBuffer response_buffer;
	Writer<StringBuffer> writer(response_buffer);

	writer.StartObject();
	writer.String("results");
	writer.StartArray();

	for(pqxx::result::size_type i = 0; i < res.size(); i++){
		writer.StartObject();
		writer.String("id");
		writer.String(res[i]["id"].as<const char *>());
		writer.String("username");
		writer.String(res[i]["username"].as<const char *>());
		writer.String("email");
		writer.String(res[i]["email"].as<const char *>());
		writer.String("first_name");
		writer.String(res[i]["first_name"].as<const char *>());
		writer.String("last_name");
		writer.String(res[i]["last_name"].as<const char *>());
		writer.EndObject();
	}

	writer.EndArray();
	writer.EndObject();

	return response_buffer.GetString();
}

int main(int argc, char** argv){
	//Tokens will only remain valid for the duration this WebService runs.
	CryptoPP::AutoSeededRandomPool rand_tool;
        rand_tool.GenerateBlock(token_key, CryptoPP::AES::MAX_KEYLENGTH);
        rand_tool.GenerateBlock(token_iv, CryptoPP::AES::BLOCKSIZE);

	try{
		WebService api(3000);

		api.route("/", root);

		api.route("/user/new", newuser,
			{ "username", "password", "email", "first_name", "last_name" });

		api.route("/users", users,
			{ "token" });

		api.route("/login",login,
			{ "username", "password" });

		api.listen();
	}
	catch (std::exception& e){
		std::cerr << "WebService exception: " << e.what() << "\n";
		std::getchar();
		return 1;
	}

	return 0;
}
