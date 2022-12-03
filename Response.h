#pragma once

#include <boost/asio.hpp>
#include "Payload.h"


using boost::asio::ip::tcp;


class ResponseHeader {

private:
	unsigned char version;
	unsigned short code;
	unsigned int payloadSize;

public:
	ResponseHeader(unsigned char ver, const unsigned short cd, const unsigned int payloadSz);
	~ResponseHeader();
	ResponseHeader(const ResponseHeader& responseHeadr);
	ResponseHeader& operator=(const ResponseHeader& responseHeadr);
	const unsigned int getPayloadSize() const;
	const unsigned short getCode() const;
	const unsigned char getVersion() const;
	void print();
};


class Response {

	ResponseHeader* responseHeader;
	ResponsePayload* payload;
public:
	Response(const Response& response);
	Response& operator=(const Response& response);
	Response(ResponseHeader* headr , const unsigned char* data);
	~Response();
	ResponsePayload* getPayload() const;
	ResponseHeader* getHeader() const;
	void print();

};

class ResponseParser {

private:
	tcp::socket* socket_;

public:
	ResponseParser(tcp::socket* s);
	~ResponseParser();

	Response* toResponse();

};