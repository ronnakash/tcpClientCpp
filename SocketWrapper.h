#pragma once

#include <boost/asio.hpp>
#include "Request.h"
#include "Response.h"


using boost::asio::ip::tcp;

class SocketWrapper 
{
public:

	SocketWrapper(std::string host, std::string port);
	~SocketWrapper();
	tcp::socket* getSocket();
	Response* sendAndReceive(Request* requestptr);
private:
	//tcp::socket* createSocket();
	void getConnection();
	void closeConnection();
	void sendRequest(Request* requestptr);
	Response* getResponse();
	tcp::socket* socket_obj_;
	std::string host_;
	std::string port_;
	boost::asio::io_service* io_service_;
	tcp::resolver* resolver_;
	tcp::resolver::query* query_;
	tcp::resolver::iterator* iterator_;
};