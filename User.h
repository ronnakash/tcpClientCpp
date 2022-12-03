#pragma once
#include <string>
#include "SocketWrapper.h"
#include "SocketWrapper.h"

using namespace std;

class User {

private:
	string id_;
	string username_;

public:
	User(string id, string username);
	~User();
	User(const User& user);
	User& operator=(const User& user);
	string getId() const;
	string getUsername() const;
	static User* registerIfNeeded(SocketWrapper* sw, std::string nameString);
	static User* registerUser(SocketWrapper* sw, std::string nameString);


};