#include "Request.h"
#include <corecrt_malloc.h>
#include <iostream>
#include <iomanip>
#include "Utils.h"


RequestHeader::RequestHeader(std::string* cid, const unsigned short reqCode, const unsigned int payloadSz)
{
	clientId = cid;
	requestCode = reqCode;
	payloadSize = payloadSz;
}

RequestHeader::~RequestHeader()
{
}

unsigned int RequestHeader::getPayloadSize()
{
	return payloadSize;
}

char* RequestHeader::toBytes()
{
	//std::string* bytearray = clientId;
	//bytearray->resize(23);
	char* cidBytes = (char *) (clientId->c_str());
	unsigned char* bytearray = new unsigned char[23];
	//client id
	for (int i = 0; i < 16; i++)
		bytearray[i] = cidBytes[i];
	//client version
	bytearray[16] = 0x03;
	//request code
	bytearray[17] = requestCode;
	bytearray[18] = requestCode >> 8;
	//payloadSize
	unsigned int temp = payloadSize;
	for (int i = 19; i < 23; i++) {
		bytearray[i] = temp;
		temp = temp >> 8;
	}
	//std::string* res = new string(bytearray);
	//return res;
	std::cout << "Request Header:";
	Utils::hexify(bytearray, 23);

	return (char *) bytearray;
}


Request::Request(std::string* cid, unsigned short reqCode,
	unsigned int payloadSize, RequestPayload* payload)
{
	requestHeader_ = new RequestHeader(cid, reqCode, payloadSize);
	payload_ = payload;

}

Request::~Request()
{
	delete requestHeader_;
	delete payload_;
}

unsigned int Request::getSize()
{
	return 23 + requestHeader_->getPayloadSize();
}

char* Request::toBytes()
{
	int size = getSize();
	int i = 0;
	unsigned char* bytearray = new unsigned char[size];
	char* headerBytes = requestHeader_->toBytes();
	unsigned char* payloadMsg = payload_->toMessage();
	for (; i < 23; i++)
		bytearray[i] = headerBytes[i];
	for (; i < size; i++)
		bytearray[i] = payloadMsg[i - 23];

	//std::cout << "Request:" << endl;
	//Utils::hexify(bytearray, size);

	return (char *) bytearray;
}
