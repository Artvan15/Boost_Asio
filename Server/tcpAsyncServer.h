#pragma once

#include <iostream>
#include <boost/asio.hpp>

#include "tcpConnection.h"


using boost::asio::ip::tcp;

class tcpAsyncServer
{
public:
	tcpAsyncServer(boost::asio::io_context& io_, unsigned short port)
		: io(io_),
		  acceptor(io_, tcp::endpoint(tcp::v4(), 13))
	{
		StartAccept();
		std::cout << "Continue listening\n";
	}

private:
	void StartAccept()
	{
		//when handler is called, new_connection must exist, thus it's member
		new_connection = tcpConnection::Create(io);
		std::cout << "Listening\n";

		// !capture by reference leads to dangling reference!
		acceptor.async_accept(new_connection->GetSocket(),
			[this](const boost::system::error_code& er)
			{
				if (!er)
				{
					new_connection->Start();
				}
				else
				{
					std::cerr << er.message() << std::endl;
				}
				StartAccept();
			});
	}

	tcpConnection::pointer new_connection;
	boost::asio::io_context& io;
	tcp::acceptor acceptor;
	
};