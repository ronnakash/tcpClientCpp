#include "User.h"
#include "FileReader.h"
#include <iostream>
#include "Utils.h"

User::User(string id, string username)
{
	id_ = id;
	username_ = username;

}

User::~User()
{
}

User::User(const User& user)
{
	id_ = user.getId();
	username_ = user.getUsername();
}

User& User::operator=(const User& user)
{
	if (this != &user) {
		id_ = user.getId();
		username_ = user.getUsername();
	}
	return *this;
}

string User::getId() const
{
	return id_;
}

string User::getUsername() const
{
	return username_;
}

User* User::registerIfNeeded(SocketWrapper* sw, std::string nameString)
{
	if (FileReader::infoFileExists()) {
		MyInfoFileDetails* myInfo = FileReader::readMyInfoFile();
		char* temp = (char*)myInfo->getClientId();
		//std::cout << "chars in rin: " << std::endl;
		//Utils::hexify((unsigned char*)temp, 16);
		std::string clientIdStr = temp;
		//std::cout << "str in rin: " << std::endl;
		//Utils::hexify(16, clientIdStr);
		std::string username = myInfo->getUsername();
		return new User(clientIdStr, username);
	}
	else {
		User* user = registerUser(sw, nameString);
		//create MyInfo file
		std::string clientIdStr = user->getId();
		std::string username = user->getUsername();
		MyInfoFileDetails* myInfo = new MyInfoFileDetails(username, (unsigned char*)clientIdStr.c_str(), nullptr);
		FileReader::createMyInfoFile(myInfo);
		return user;
	}

}

User* User::registerUser(SocketWrapper* sw, std::string nameString)
{
	std::string clientId("ignored");
	RegisterRequestPayload* registerReqPayload = new RegisterRequestPayload(nameString);
	Request* registerRequest = new Request(&clientId, RequestHeader::REGISTER_REQUEST_CODE, 255, (RequestPayload*)registerReqPayload);
	Response* response = sw->sendAndReceive(registerRequest);
	response->print();
	//TODO: make sure registration worked
	unsigned short responseCode = response->getHeader()->getCode();
	if (responseCode == 2001)
		throw std::runtime_error("Error: Registration failed on server");
	std::string* newClientId = new std::string(((RegisterResponsePayload*)response->getPayload())->getClientId());
	return new User(*newClientId, nameString);

}
