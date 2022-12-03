#include <boost/asio.hpp>
#include "SocketWrapper.h"
#include <iostream>
#include "FileReader.h"


using boost::asio::ip::tcp;


SocketWrapper::SocketWrapper(std::string host, std::string port)
{
	host_ = host;
	port_ = port;
}


SocketWrapper::~SocketWrapper()
{
	delete socket_obj_;
}

void SocketWrapper::sendRequest(Request* requestptr)
{
	char* message = requestptr->toBytes();
	boost::asio::write(*socket_obj_, boost::asio::buffer(message, requestptr->getSize()));

}

Response* SocketWrapper::getResponse()
{
	ResponseParser* parser = new ResponseParser(socket_obj_);
	return parser->toResponse();
}

tcp::socket* SocketWrapper::getSocket()
{
	return socket_obj_;
}

Response* SocketWrapper::sendAndReceive(Request* requestptr)
{
	//boost::asio::io_service io_service;
	//tcp::resolver resolver(io_service);
	//tcp::resolver::query query(tcp::v4(), host_, port_);
	//tcp::resolver::iterator iterator = resolver.resolve(query);
	//tcp::socket s(io_service);
	//boost::asio::connect(s, iterator);
	getConnection();
	sendRequest(requestptr);
	Response* res = getResponse();
	closeConnection();
	return res;

}

void SocketWrapper::getConnection()
{
	io_service_ = new boost::asio::io_service();
	//boost::asio::io_service ios = *io_service_;
	resolver_ = new tcp::resolver(*io_service_);
	query_ = new tcp::resolver::query(tcp::v4(), host_, port_);
	iterator_ = new tcp::resolver::iterator(resolver_->resolve(*query_));
	socket_obj_ = new tcp::socket(*io_service_);
	boost::asio::connect(*socket_obj_, *iterator_);
	//return s;
}

void SocketWrapper::closeConnection()
{
	delete io_service_;
	delete resolver_;
	delete query_;
	delete iterator_;
	delete socket_obj_;

}




//tcp::socket* SocketWrapper::createSocket()
//{
//	try {
//		boost::asio::io_service ioService;
//		tcp::resolver resolver(ioService);
//		tcp::resolver::query query(tcp::v4(), "127.0.0.1", "65432");
//		tcp::resolver::iterator iterator = resolver.resolve(query);
//		tcp::socket s(ioService);
//		boost::asio::connect(s, iterator);
//		return &s;
//	}
//	catch (std::exception& e) {
//		std::cerr << "Exception1: " << e.what() << "\n";
//	}
//}
