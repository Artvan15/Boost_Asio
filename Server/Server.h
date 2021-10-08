#pragma once

#include <ctime>
#include <iostream>
#include <string>

#include "CurrentDate.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;


class Server
{
public:
	//acceptor listen for connections of all IPv4 addresses and chosen port
	Server(unsigned short port)
		: acceptor(io, tcp::endpoint(tcp::v4(), port)) {}

	void Run()
	{
		try
		{
			std::cout << "Listening\n";
			tcp::socket socket(io);

			//start listening. Current thread is waiting for connection
			acceptor.accept(socket);
			boost::system::error_code error;
			socket.write_some(boost::asio::buffer(GetCurrentDate()), error);
		}
		catch (std::exception& ex)
		{
			std::cerr << ex.what() << std::endl;
		}
	}

	~Server()
	{
		std::cout << "Close server" << std::endl;
	}

private:	
	boost::asio::io_context io;
	tcp::acceptor acceptor;
};