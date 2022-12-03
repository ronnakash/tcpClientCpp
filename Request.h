#pragma once
#include <string>
#include "Payload.h"

class RequestHeader
{

public:
	static const unsigned short REGISTER_REQUEST_CODE = 1100;
	static const unsigned short SEND_PUBLIC_KEY_REQUEST_CODE = 1101;
	static const unsigned short SEND_FILE_REQUEST_CODE = 1103;
	static const unsigned short CRC_OK_REQUEST_CODE = 1104;
	static const unsigned short CRC_RETRY_REQUEST_CODE = 1105;
	static const unsigned short CRC_FAIL_REQUEST_CODE = 1106;

private:
	std::string* clientId;
	unsigned int requestCode;
	unsigned int payloadSize;

public:

	RequestHeader(std::string* cid, const unsigned short reqCode,
		const unsigned int payloadSize);

	~RequestHeader();

	unsigned int getPayloadSize();

	char* toBytes();

};

class Request
{

private:

	RequestHeader* requestHeader_;
	RequestPayload* payload_;


public:
	Request(std::string* cid, unsigned short reqCode,
		unsigned int payloadSize, RequestPayload* payload);
	~Request();

	unsigned int getSize();

	char* toBytes();

};