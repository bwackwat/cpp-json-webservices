#include <string>
#include <sstream>
#include <iostream>
#include <thread>

#include "argon2.h"

#include "cryptopp/aes.h"
#include "cryptopp/osrng.h"
#include "cryptopp/modes.h"
#include "cryptopp/base64.h"
#include "cryptopp/filters.h"
#include "cryptopp/cryptlib.h"

#include "util.hpp"

//Tokens will only remain valid for the duration this WebService runs.
byte* token_key = new byte[CryptoPP::AES::MAX_KEYLENGTH];
byte* token_iv = new byte[CryptoPP::AES::BLOCKSIZE];
std::string nonce;

void init_crypto(std::string salt){
	CryptoPP::AutoSeededRandomPool rand_tool;

	rand_tool.GenerateBlock(token_key, CryptoPP::AES::MAX_KEYLENGTH);
	rand_tool.GenerateBlock(token_iv, CryptoPP::AES::BLOCKSIZE);

	nonce = salt;
}

std::string hash_password(std::string password){
	const uint32_t t_cost = 5;
	const uint32_t m_cost = 1 << 16; //about 65MB
	const uint32_t parallelism = 1; //TODO: can use std::thread::hardware_concurrency();?

	std::vector<uint8_t> pwdvec(password.begin(), password.end());
	uint8_t* pwd = &pwdvec[0];
	const size_t pwdlen = password.length();

	//TODO: Should this be used??
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
		hash, hashlen, encoded, encodedlen, type, ARGON2_VERSION_NUMBER);
	
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

std::string pretty(std::string str){
	int depth = 0;
	std::stringstream nit;
	char tab = ' ';
	for(std::string::size_type i = 0; i < str.size(); ++i) {
		if(str[i] == '{'){
			nit << "" << std::string(depth * 2, tab);
			nit << "{\n" << std::string(++depth * 2, tab);
		}else if(str[i] == '['){
			nit << "[\n" << std::string(depth++ * 2, tab);
		}else if(str[i] == ','){
			nit << ",\n" << std::string(depth * 2, tab);
		}else if(str[i] == ']'){
			nit << "\n]" << std::string(depth-- * 2, tab);
		}else if(str[i] == '}'){
			nit << "\n}" << std::string(depth-- * 2, tab);
		}else{
			nit << str[i];
		}
	}
	return nit.str();
}
