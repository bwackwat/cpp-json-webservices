#include <string>
#include <iostream>
#include <thread>

//#include "rapidjson/document.h"
//#include "rapidjson/writer.h"
//#include "rapidjson/stringbuffer.h"

#include "argon2.h"

#include "cryptopp/aes.h"
#include "cryptopp/osrng.h"
#include "cryptopp/modes.h"
#include "cryptopp/base64.h"
#include "cryptopp/filters.h"
#include "cryptopp/cryptlib.h"

#include "util.hpp"

//using namespace rapidjson;

//Tokens will only remain valid for the duration this WebService runs.
byte* token_key = new byte[CryptoPP::AES::MAX_KEYLENGTH];
byte* token_iv = new byte[CryptoPP::AES::BLOCKSIZE];

void init_crypto(){
	CryptoPP::AutoSeededRandomPool rand_tool;

	rand_tool.GenerateBlock(token_key, CryptoPP::AES::MAX_KEYLENGTH);
	rand_tool.GenerateBlock(token_iv, CryptoPP::AES::BLOCKSIZE);
}

std::string simple_error_json(std::string message){
	return "{\"error\":\"" + message + "\"}";
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

/* SCRAPYARD!
void _log(std::string logdata){
	std::cout << std::this_thread::get_id() << ": " << logdata << std::endl;
}

std::string json_error(std::string msg){
	/// ALTERNATIVE:

	StringBuffer s;
	Writer<StringBuffer> writer(s);

	writer.StartObject();
	writer.String("error");
	writer.String(msg);
	writer.EndObject();

	return s.GetString();

	

	Document response_doc;
	StringBuffer response_doc_buffer;
	//response_doc_buffer.Clear();
	Writer<StringBuffer> writer(response_doc_buffer);

	response_doc.SetObject();
	Value res(msg.c_str(), response_doc.GetAllocator());
	response_doc.AddMember("error", res, response_doc.GetAllocator());
	response_doc.Accept(writer);

	return response_doc_buffer.GetString();
}*/