#pragma once

#include <string>

#include <pqxx/pqxx>

class PostgresRepository {
public:
	PostgresRepository(std::string connection_string);

	pqxx::result GetUsers();
	pqxx::result GetUserById(std::string id);
	pqxx::result GetUserByUsername(std::string username);
	pqxx::result GetUserByEmail(std::string email);
	pqxx::result GetUserByLogin(std::string login);
	pqxx::result GetUserByUsernameOrEmail(std::string username, std::string email);
	
	pqxx::result CreateUser(std::string username, std::string password, std::string email, std::string first_name, std::string last_name);

private:
	pqxx::connection conn;
	
	pqxx::result SQLWrap(pqxx::work* txn, std::string sql);
};