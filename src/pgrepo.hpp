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
	
	pqxx::result GetBlogPostsByUserId(std::string id);
	pqxx::result CreateBlogPost(std::string username, std::string title, std::string text);
	pqxx::result UpdateBlogPost(std::string id, std::string title, std::string content);

	pqxx::result GetPoiByUserId(std::string id);
	pqxx::result CreatePoi(std::string owner, std::string label, std::string description, double longitude, double latitude);

private:
	pqxx::connection conn;
	
	pqxx::result SQLWrap(pqxx::work* txn, std::string sql);
};