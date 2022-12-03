#pragma once
#include <string>
#include <vector>

using namespace std;


class Payload {

	
};

class ResponsePayload : private Payload
{
public:
	virtual void print() = 0;
};

class RequestPayload : private Payload 
{

public:
	virtual unsigned char* toMessage();
};

class EmptyResponsePayload : private ResponsePayload
{
public:
	EmptyResponsePayload();
	~EmptyResponsePayload();
	EmptyResponsePayload(const EmptyResponsePayload& emptyResPayload);
	EmptyResponsePayload& operator=(const EmptyResponsePayload& emptyResPayload);

	void print();
};

class RegisterRequestPayload : private RequestPayload {

private:
	string username_;

public:
	RegisterRequestPayload(string username);
	RegisterRequestPayload(const RegisterRequestPayload& regResPayload);
	RegisterRequestPayload& operator=(const RegisterRequestPayload& regResPayload);
	string getUsername() const;
	unsigned char* toMessage();
};

class KeyExchangeRequestPayload : private RequestPayload {

private:
	string username_;
	string publicKey_;

public:
	KeyExchangeRequestPayload(string username, string publicKey);
	~KeyExchangeRequestPayload();
	KeyExchangeRequestPayload(const KeyExchangeRequestPayload& keyExResPayload);
	KeyExchangeRequestPayload& operator=(const KeyExchangeRequestPayload& keyExResPayload);
	string getUsername() const;
	string getKey() const;
	unsigned char* toMessage();
};

class FileExchangeRequestPayload : private RequestPayload {

protected:
	string clientId_;
	unsigned int contentSize_;
	string filename_;
	string fileContent_;

public:
	FileExchangeRequestPayload(string clientId,
		unsigned int contentSize, string filename,
		string fileContent);
	FileExchangeRequestPayload(const FileExchangeRequestPayload& fileExResPayload);
	FileExchangeRequestPayload& operator=(const FileExchangeRequestPayload& fileExResPayload);
	unsigned int getSize() const;
	~FileExchangeRequestPayload();
	unsigned char* toMessage();
};

class crcRequestPayload : private RequestPayload {

private:
	string clientId_;
	string filename_;

public:
	crcRequestPayload(string clientId, string filename);
	~crcRequestPayload();
	crcRequestPayload(const crcRequestPayload& crcResPayload);
	crcRequestPayload& operator=(const crcRequestPayload& crcResPayload);

	unsigned char* toMessage();
};

class RegisterResponsePayload : ResponsePayload {

protected:
	const unsigned char* clientId_;

public:
	RegisterResponsePayload(const unsigned char* clientId);
	~RegisterResponsePayload();
	RegisterResponsePayload(const RegisterResponsePayload& regResPayload);
	RegisterResponsePayload& operator=(const RegisterResponsePayload& regResPayload);

	string getClientId();
	void print();
};

class KeyExchangeResponsePayload : ResponsePayload{

protected:
	const unsigned char* clientId_;
	const unsigned char* encryptedKey_;

public:
	KeyExchangeResponsePayload(const unsigned char* clientId, const unsigned char* encryptedKey);
	~KeyExchangeResponsePayload();
	//string getClientId();
	const unsigned char* getEncryptedKey() const;
	KeyExchangeResponsePayload(const KeyExchangeResponsePayload& keyExResPayload);
	KeyExchangeResponsePayload& operator=(const KeyExchangeResponsePayload& keyExResPayload);
	void print();
};

class FileExchangeResponsePayload : ResponsePayload {

private:
	string clientId_;
	unsigned int filesize_;
	string filename_;
	unsigned int checksum_;


public:
	FileExchangeResponsePayload(string clientId,
		unsigned int filesize, string filename,
		unsigned int checksum);
	~FileExchangeResponsePayload();
	FileExchangeResponsePayload(const FileExchangeResponsePayload& fileExResPayload);
	FileExchangeResponsePayload& operator=(const FileExchangeResponsePayload& fileExResPayload);

	string getClientId() const;
	string getFileName() const;
	unsigned int getChecksum() const;
	unsigned int getfileSize() const;
	void print();
};

class PayloadParser {


protected:
	const unsigned char* payload_;
	unsigned short code_;
	unsigned int payloadSize_;


public:
	PayloadParser(const unsigned char* payload, unsigned short code, unsigned int payloadSize);
	~PayloadParser();
	PayloadParser();
};


//class RequestPayloadParser {
//
//private:
//	string payload_;
//	unsigned short code_;
//
//public:
//	RequestPayloadParser(string payload, unsigned short code);
//	~RequestPayloadParser();
//	RequestPayload* parse();
//};

class ResponsePayloadParser : private PayloadParser{

//protected:
//	string payload_;
//	unsigned short code_;

private:
	KeyExchangeResponsePayload* parseKeyExchange();
	FileExchangeResponsePayload* parseFileExchange();


public:
	ResponsePayloadParser(const unsigned char* payload,
		unsigned short code, unsigned int payloadSize) : PayloadParser(payload, code, payloadSize) {};
	~ResponsePayloadParser();
	ResponsePayloadParser(const ResponsePayloadParser& responsePayloadParser);
	ResponsePayloadParser& operator=(const ResponsePayloadParser& responsePayloadParser);

	ResponsePayload* parse();

};

//class ClientId {
//
//private:
//	char* data_;
//
//public:
//	ClientId(char* data);
//	~ClientId();
//	string toString();
//};