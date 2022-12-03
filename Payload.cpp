#include "Payload.h"
#include <iostream>
#include <iomanip>

#include "Utils.h"

KeyExchangeResponsePayload* ResponsePayloadParser::parseKeyExchange()
{
	const unsigned char* bytes = payload_;
	unsigned char* cidBytes = new unsigned char[17];
	unsigned char* keyBytes = new unsigned char[payloadSize_ - 15];
	unsigned int i = 0;
	for (; i < 16; i++) {
		char temp = bytes[i];
		cidBytes[i] = temp;
	}
	cidBytes[16] = 0;
	for (i = 16; i < payloadSize_; i++) {
		char temp = bytes[i];
		keyBytes[i - 16] = temp;
	}
	keyBytes[payloadSize_ - 16] = 0x00;
	return new KeyExchangeResponsePayload(cidBytes, keyBytes);
}

FileExchangeResponsePayload* ResponsePayloadParser::parseFileExchange()
{
	const unsigned char* bytes = payload_;
	std::cout << "payload";

	char* cidBytes = new char[17];
	unsigned int fileSize = 0;
	char* filenameBytes = new char[256];
	unsigned int checksum = 0;
	int i = 0;
	for (; i < 16; i++)
		cidBytes[i] = bytes[i];
	cidBytes[16] = 0;
	for (; i < 20; i++) {
		unsigned char temp = bytes[i];
		fileSize += temp << ((i - 16)*8);
	}
	for (; i < 275; i++)
		filenameBytes[i-20] = bytes[i];
	filenameBytes[255] = 0x00;
	for (; i < 279; i++) {
		unsigned char temp = bytes[i];
		checksum += temp << ((i - 275) * 8);
	}
	string cidString(cidBytes);
	string filenameString(filenameBytes);
	return new FileExchangeResponsePayload(cidString, fileSize, filenameString, checksum);
}

ResponsePayloadParser::~ResponsePayloadParser()
{
}

ResponsePayloadParser::ResponsePayloadParser(const ResponsePayloadParser& responsePayloadParser)
{
	payload_ = responsePayloadParser.payload_;
	code_ = responsePayloadParser.code_;
	payloadSize_ = responsePayloadParser.payloadSize_;

}

ResponsePayloadParser& ResponsePayloadParser::operator=(const ResponsePayloadParser& responsePayloadParser)
{
	if (this != &responsePayloadParser) {
		payload_ = responsePayloadParser.payload_;
		code_ = responsePayloadParser.code_;
		payloadSize_ = responsePayloadParser.payloadSize_;
	}
	return *this;
}

ResponsePayload* ResponsePayloadParser::parse()
{
	std::cout << "Raw Payload while parsing response:" << std::endl;
	Utils::hexify(payload_, payloadSize_);
	switch (code_)
	{
	case 2100:
		return (ResponsePayload*) new RegisterResponsePayload(payload_);
		break;
	case 2101:
		//error for register fail
		return (ResponsePayload*) new EmptyResponsePayload();
		break;
	case 2102:
		return (ResponsePayload*) parseKeyExchange();
		break;
	case 2103:
		return (ResponsePayload*) parseFileExchange();
		break;
	case 2104:
		return (ResponsePayload*) new EmptyResponsePayload();
		break;
	default:
		//throw error

		break;
	}
	return nullptr;
}



PayloadParser::PayloadParser(const unsigned char* payload, unsigned short code, unsigned int payloadSize)
{
	payload_ = payload;
	code_ = code;
	payloadSize_ = payloadSize;
}

PayloadParser::~PayloadParser()
{
}

PayloadParser::PayloadParser()
{
	//payload_ = string();
	//code_ = 0;
	//payloadSize_ = 0;

}

RegisterResponsePayload::RegisterResponsePayload(const unsigned char* clientId)
{
	clientId_ = clientId;
}

RegisterResponsePayload::~RegisterResponsePayload()
{
}

RegisterResponsePayload::RegisterResponsePayload(const RegisterResponsePayload& regResPayload)
{
	clientId_ = regResPayload.clientId_;
}

string RegisterResponsePayload::getClientId()
{
	return std::string((char*) clientId_);
}

void RegisterResponsePayload::print()
{
	//Hexify
	std::cout << "ClientId:";
	Utils::hexify(clientId_, 16);
}

RegisterRequestPayload::RegisterRequestPayload(string username)
{
	username_ = username;
}


RegisterRequestPayload::RegisterRequestPayload(const RegisterRequestPayload& regResPayload)
{
}

RegisterRequestPayload& RegisterRequestPayload::operator=(const RegisterRequestPayload& regResPayload)
{
	if (this != &regResPayload) {
		username_ = regResPayload.getUsername();
	}
	return *this;
}

string RegisterRequestPayload::getUsername() const
{
	return username_;
}

unsigned  char* RegisterRequestPayload::toMessage()
{
	unsigned  char* msg = new unsigned char[255];
	int len = username_.length();
	for (int i = 0; i < 255; i++) {
		msg[i] = i < len ? username_[i] : 0x00;
	}
	return msg;
}

KeyExchangeResponsePayload::KeyExchangeResponsePayload(const unsigned char* clientId, const unsigned char* encryptedKey)
{
	clientId_ = clientId;
	encryptedKey_ = encryptedKey;
}

KeyExchangeResponsePayload::~KeyExchangeResponsePayload()
{
}

//string KeyExchangeResponsePayload::getClientId()
//{
//	return clientId_;
//}
//

const unsigned char* KeyExchangeResponsePayload::getEncryptedKey() const
{
	return encryptedKey_;
}

KeyExchangeResponsePayload::KeyExchangeResponsePayload(const KeyExchangeResponsePayload& keyExResPayload)
{
	clientId_ = keyExResPayload.clientId_;
	encryptedKey_ = keyExResPayload.encryptedKey_;

}

KeyExchangeResponsePayload& KeyExchangeResponsePayload::operator=(const KeyExchangeResponsePayload& keyExResPayload)
{
	if (this != &keyExResPayload) {
		clientId_ = keyExResPayload.clientId_;
		encryptedKey_ = keyExResPayload.encryptedKey_;
	}
	return *this;
}

void KeyExchangeResponsePayload::print()
{
	//std::cout << "Payload:" << endl;
	//std::cout << "Client ID: ";
	//Utils::hexify(clientId_, 16);
	//std::cout << "Encrypted Key: ";
	//Utils::hexify(encryptedKey_, 144-16);
}

FileExchangeResponsePayload::FileExchangeResponsePayload(string clientId, unsigned int filesize, string filename, unsigned int checksum)
{
	clientId_ = clientId;
	filesize_ = filesize;
	filename_ = filename;
	checksum_ = checksum;
}

FileExchangeResponsePayload::~FileExchangeResponsePayload()
{
}

FileExchangeResponsePayload::FileExchangeResponsePayload(const FileExchangeResponsePayload& fileExResPayload)
{
	clientId_ = fileExResPayload.getClientId();
	filesize_ = fileExResPayload.getfileSize();
	filename_ = fileExResPayload.getFileName();
	checksum_ = fileExResPayload.getChecksum();

}

FileExchangeResponsePayload& FileExchangeResponsePayload::operator=(const FileExchangeResponsePayload& fileExResPayload)
{
	if (this != &fileExResPayload) {
		clientId_ = fileExResPayload.getClientId();
		filesize_ = fileExResPayload.getfileSize();
		filename_ = fileExResPayload.getFileName();
		checksum_ = fileExResPayload.getChecksum();
	}
	return *this;
}

string FileExchangeResponsePayload::getClientId() const
{
	return clientId_;
}

string FileExchangeResponsePayload::getFileName() const
{
	return filename_;
}

unsigned int FileExchangeResponsePayload::getChecksum() const
{
	return checksum_;
}

unsigned int FileExchangeResponsePayload::getfileSize() const
{
	return filesize_;
}

void FileExchangeResponsePayload::print()
{
	//std::cout << "Payload:" << endl;
	std::cout << "Client ID: ";
	Utils::hexify(16, clientId_);
	std::cout << "File Name: " << filename_ << endl;
	std::cout << "File Size: " << filesize_ << endl;
	std::cout << "Checksum: " << checksum_ << endl;
}

KeyExchangeRequestPayload::KeyExchangeRequestPayload(string username, string publicKey)
{
	username_ = username;
	publicKey_ = publicKey;
}

KeyExchangeRequestPayload::~KeyExchangeRequestPayload()
{
}

KeyExchangeRequestPayload::KeyExchangeRequestPayload(const KeyExchangeRequestPayload& keyExResPayload)
{
	username_ = keyExResPayload.getUsername();
	publicKey_ = keyExResPayload.getKey();
}

KeyExchangeRequestPayload& KeyExchangeRequestPayload::operator=(const KeyExchangeRequestPayload& keyExResPayload)
{
	if (this != &keyExResPayload) {
		username_ = keyExResPayload.getUsername();
		publicKey_ = keyExResPayload.getKey();
	}
	return *this;
}

string KeyExchangeRequestPayload::getUsername() const
{
	return username_;
}

string KeyExchangeRequestPayload::getKey() const
{
	return publicKey_;
}

unsigned char* KeyExchangeRequestPayload::toMessage()
{
	unsigned char* message = new unsigned char[255 + 160];
	for (unsigned int i = 0; i < 255; i++) {
		message[i] = i < username_.length() ? username_[i] : 0;
	}
	for (int i = 0; i < 160; i++) {
		//char temp = publicKey_[i];
		message[i+255] = publicKey_[i];
	}

	//std::ios::fmtflags f(std::cout.flags());
	//std::cout << std::hex;
	//for (size_t i = 0; i < 415; i++)
	//	std::cout << std::setfill('0') << std::setw(2) << (0xFF & message[i]) << (((i + 1) % 16 == 0) ? "\n" : " ");
	//std::cout << std::endl;
	//std::cout.flags(f);

	return message;
}



FileExchangeRequestPayload::FileExchangeRequestPayload(string clientId, unsigned int contentSize, string filename, string fileContent)
{
	clientId_ = clientId;
	contentSize_ = contentSize;
	filename_ = filename;
	fileContent_ = fileContent;
}

FileExchangeRequestPayload::FileExchangeRequestPayload(const FileExchangeRequestPayload& fileExResPayload)
{
}

FileExchangeRequestPayload& FileExchangeRequestPayload::operator=(const FileExchangeRequestPayload& fileExResPayload)
{
	if (this != &fileExResPayload) {
		clientId_ = fileExResPayload.clientId_;
		contentSize_ = fileExResPayload.contentSize_;
		filename_ = fileExResPayload.filename_;
		fileContent_ = fileExResPayload.fileContent_;
	}
	return *this;
}

unsigned int FileExchangeRequestPayload::getSize() const
{
	return contentSize_ + 275;
}

FileExchangeRequestPayload::~FileExchangeRequestPayload()
{
}

unsigned char* FileExchangeRequestPayload::toMessage()
{
	unsigned char* cidBytes = (unsigned char*)clientId_.c_str();
	unsigned char* filenameBytes = (unsigned char*)filename_.c_str();
	unsigned char* fileContentBytes = (unsigned char*)fileContent_.c_str();
	unsigned char* message = new unsigned char[275 + contentSize_];
	int i = 0;
	int namelen = filename_.length();
	unsigned int temp = contentSize_;
	//std::cout << "clientId bytes:" << std::endl;
	//Utils::hexify(cidBytes, 16);
	//std::cout << "filename bytes:" << std::endl;
	//Utils::hexify(filenameBytes, namelen);

	for (; i < 16; i++) {
		message[i] = cidBytes[i];
	}
	for (; i < 20; i++) {
		message[i] = temp;
		temp = temp >> 8;
	}
	for (i; i < 275; i++)
		message[i] = i < namelen + 20 ? filenameBytes[i-20] : 0x00;
	for (i; i < 275 + contentSize_; i++)
		message[i] = fileContentBytes[i - 275];

	//std::cout << "FileExchangeRequestPayload:" << std::endl;
	//Utils::hexify(message, 275 + contentSize_);
	return message;
}

crcRequestPayload::crcRequestPayload(string clientId, string filename)
{
	clientId_ = clientId;
	filename_ = filename;
}

crcRequestPayload::~crcRequestPayload()
{
}

unsigned char* crcRequestPayload::toMessage()
{
	unsigned char* cidBytes = (unsigned char*)clientId_.c_str();
	unsigned char* filenameBytes = (unsigned char*)filename_.c_str();
	unsigned char* message = new unsigned char[255+16];
	int i = 0;
	int namelen = filename_.length();
	//std::cout << "clientId bytes:" << std::endl;
	//Utils::hexify(cidBytes, 16);
	//std::cout << "filename bytes:" << std::endl;
	//Utils::hexify(filenameBytes, namelen);
	for (; i < 16; i++) {
		message[i] = cidBytes[i];
	}
	for (i; i < 255+16; i++)
		message[i] = i < namelen + 16 ? filenameBytes[i - 16] : 0x00;
	//std::cout << "FileExchangeRequestPayload:" << std::endl;
	//Utils::hexify(message, 255 + 16);
	return message;

}


unsigned char* RequestPayload::toMessage()
{
	std::cout << "RequestPayload::toMessage()" << std::endl;
	return nullptr;
}

EmptyResponsePayload::EmptyResponsePayload()
{
}

EmptyResponsePayload::~EmptyResponsePayload()
{
}

EmptyResponsePayload::EmptyResponsePayload(const EmptyResponsePayload& emptyResPayload)
{
}

EmptyResponsePayload& EmptyResponsePayload::operator=(const EmptyResponsePayload& emptyResPayload)
{
	return *this;
}

void EmptyResponsePayload::print()
{
	std::cout << "Payload is empty" << endl;
}

