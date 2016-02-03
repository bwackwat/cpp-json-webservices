DROP TABLE users;

CREATE TABLE users (
	id serial PRIMARY KEY,
	username varchar(50) UNIQUE NOT NULL,
	password text NOT NULL,
	email varchar(100) UNIQUE NOT NULL,
	first_name varchar(50) NOT NULL,
	last_name varchar(50) NOT NULL
);
