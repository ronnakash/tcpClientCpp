#include "Response.h"
#include <cstdlib>
#include <iostream>

#include <boost/asio.hpp>
#include "Utils.h"


using boost::asio::ip::tcp;

ResponseParser::ResponseParser(tcp::socket* s)
{
    socket_ = s;
}

ResponseParser::~ResponseParser()
{
}

Response* ResponseParser::toResponse()
{
    char* headerData = new char[7];
    size_t header_length = boost::asio::read(*socket_,
	    boost::asio::buffer(headerData, 7));
    std::cout << "Response Header Bytes: ";
    Utils::hexify((unsigned char *) headerData, 7);
    unsigned char version = headerData[0];
    unsigned short temp = ((unsigned short)headerData[2]) << 8;
    const unsigned short code = temp | headerData[1];
    unsigned int payloadSize = 0;
    for (int j = 0; j < 4; j++) {
        unsigned char temp = headerData[j+3];
        unsigned int tempInt = temp << (j * 8);
        payloadSize += tempInt;
    }
    //char* payload = new char[payloadSize+1];
    //for (int i = 0; i < payloadSize; i++)
    //    payload[i] = headerData[i + 7];
    //payload[payloadSize] = 0;
    unsigned char* payload = new unsigned char[payloadSize];
    size_t payloadLength = boost::asio::read(*socket_,
        boost::asio::buffer(payload, payloadSize));
    std::cout << "Response Payload Bytes: ";
    Utils::hexify(payload, payloadLength);
    ResponseHeader* responseHeader = new ResponseHeader(version, code, payloadSize);
    Response* response = new Response(responseHeader, payload);
    return response;
}

void Response::print()
{
    std::cout << "Response:" << std::endl;
    responseHeader->print();
    std::cout << "Payload:" << endl;
    payload->print();
}

ResponseHeader::ResponseHeader(unsigned char ver, const unsigned short cd, const unsigned int payloadSz)
{
    version = ver;
    code = cd;
    payloadSize = payloadSz;

}

ResponseHeader::~ResponseHeader()
{
}

ResponseHeader::ResponseHeader(const ResponseHeader& responseHeadr)
{
    version = 3;
    code = responseHeadr.getCode();
    payloadSize = responseHeadr.getPayloadSize();

}

ResponseHeader& ResponseHeader::operator=(const ResponseHeader& responseHeadr)
{
    if (this != &responseHeadr) {
        version = 3;
        code = responseHeadr.getCode();
        payloadSize = responseHeadr.getPayloadSize();
    }
    return *this;
}

const unsigned short ResponseHeader::getCode() const
{
    return code;
}

const unsigned char ResponseHeader::getVersion() const
{
    return version;
}

const unsigned int ResponseHeader::getPayloadSize () const
{
    return payloadSize;
}

void ResponseHeader::print()
{
    std::cout << "Header: " << ((unsigned int) getVersion()) << ", code: ";
    std::cout << getCode() << ", payloadSize: ";
    std::cout << getPayloadSize() << std::endl;
}

Response::Response(const Response& response)
{
    responseHeader = response.getHeader();
    payload = response.getPayload();
}

Response& Response::operator=(const Response& response)
{
    if (this != &response) {
        responseHeader = response.getHeader();
        payload = response.getPayload();
    }
    return *this;
}

Response::Response(ResponseHeader* headr, const unsigned char* data)
{
    responseHeader = headr;
    payload = ResponsePayloadParser(data, responseHeader->getCode(),
        responseHeader->getPayloadSize()).parse();
}

Response::~Response()
{
    delete responseHeader;
    delete payload;
}

ResponsePayload* Response::getPayload() const
{
    return payload;
}

ResponseHeader* Response::getHeader() const
{
    return responseHeader;
}
