#pragma once

#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include "CurrentDate.h"

using boost::asio::ip::tcp;

class tcpConnection : public std::enable_shared_from_this<tcpConnection>
{
	struct IsPrivate;
public:
	using pointer = std::shared_ptr<tcpConnection>;
	
	//private constructor
	tcpConnection(IsPrivate, boost::asio::io_context& io)
		: socket(io) {}

	static pointer Create(boost::asio::io_context& io)
	{
		return std::make_shared<tcpConnection>(IsPrivate(), io);
	}

	tcp::socket& GetSocket()
	{
		return socket;
	}

	//asynchroniusly write data into socket
	void Start()
	{
		message = GetCurrentDate();

		//vars are captured when lambda is initialized
		//having shared_ptr pointing to this, makes this object accessible till handler called
		boost::asio::async_write(socket, boost::asio::buffer(message),
			[ptr{ shared_from_this() }](const boost::system::error_code& ec, size_t bytes_transferred)
			{
				std::cout << "Successfully write " << bytes_transferred << std::endl;
			});
		
	}


private:
	struct IsPrivate
	{
		explicit IsPrivate() = default;
	};
	
	
	tcp::socket socket;
	std::string message;
};