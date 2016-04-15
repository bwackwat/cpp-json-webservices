#include <string>
#include <iostream>

#include <pqxx/pqxx>

#include "pgrepo.hpp"

PostgresRepository::PostgresRepository(std::string connection_string)
	:conn(connection_string){}
	
pqxx::result PostgresRepository::SQLWrap(pqxx::work* txn, std::string sql){
	std::cout << "PSQL: |" << sql << "|" << std::endl;
	pqxx::result res = txn->exec(sql);
	txn->commit();
	return res;
}
	
pqxx::result PostgresRepository::GetUsers(){
	pqxx::work txn(conn);
	return SQLWrap(&txn, "SELECT * FROM users;");
}

pqxx::result PostgresRepository::GetUserById(std::string id){
	pqxx::work txn(conn);
	return SQLWrap(&txn, "SELECT * FROM users WHERE "
		"id = " + txn.quote(id) + ";");
}

pqxx::result PostgresRepository::GetUserByUsername(std::string username){
	pqxx::work txn(conn);
	return SQLWrap(&txn, "SELECT * FROM users WHERE "
		"username = " + txn.quote(username) + ";");
}

pqxx::result PostgresRepository::GetUserByEmail(std::string email){
	pqxx::work txn(conn);
	return SQLWrap(&txn, "SELECT * FROM users WHERE "
		"email = " + txn.quote(email) + ";");
}

pqxx::result PostgresRepository::GetUserByLogin(std::string login){
	pqxx::work txn(conn);
	return SQLWrap(&txn, "SELECT * FROM users WHERE "
		"username = " + txn.quote(login) + " OR "
		"email = " + txn.quote(login) + ";");
}

pqxx::result PostgresRepository::GetUserByUsernameOrEmail(std::string username, std::string email){
	pqxx::work txn(conn);
	return SQLWrap(&txn, "SELECT * FROM users WHERE "
		"username = " + txn.quote(username) + " OR "
		"email = " + txn.quote(email) + ";");
}

pqxx::result PostgresRepository::CreateUser(std::string username, std::string password, std::string email, std::string first_name, std::string last_name){
	pqxx::work txn(conn);
	return SQLWrap(&txn, "INSERT INTO users"
		"(id, username, password, email, first_name, last_name) "
		"VALUES (DEFAULT, " +
		txn.quote(username) + ", " +
		txn.quote(password) + ", " +
		txn.quote(email) + ", " +
		txn.quote(first_name) + ", " +
		txn.quote(last_name) +
		") RETURNING id;");
}

pqxx::result PostgresRepository::GetPoiByUserId(std::string id){
	pqxx::work txn(conn);
	return SQLWrap(&txn, "SELECT label, description, ST_AsGeoJSON(location) as location, created_on FROM poi WHERE "
		"owner_id = " + txn.quote(id) + ";");
}

pqxx::result PostgresRepository::CreatePoi(std::string owner, std::string label, std::string description, double longitude, double latitude){
	pqxx::work txn(conn);
	return SQLWrap(&txn, "INSERT INTO poi"
		"(id, owner_id, label, description, location) "
		"VALUES (DEFAULT, " +
		txn.quote(owner) + ", " +
		txn.quote(label) + ", " +
		txn.quote(description) + ", " +
		"ST_GeographyFromText('POINT(" + std::to_string(longitude) + " " + std::to_string(latitude) + ")') " +
		");");
}