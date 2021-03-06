#pragma once

#include <string>

void init_crypto(std::string salt);
std::string hash_password(std::string password);
std::string encrypt_to_webtoken(std::string data);
std::string decrypt_from_webtoken(std::string token);

std::string simple_error_json(std::string message);
std::string pretty(std::string str);
