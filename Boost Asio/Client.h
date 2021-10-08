#pragma once

#include <iostream>
#include <array>
#include <boost/asio.hpp>


using boost::asio::ip::tcp;

/*
	 * We need to turn the server name into TCP endpoint. To do this we use
	 * an ip::tcp::resolver.
	 * TCP endpoint - address for connection with port
	 *
	 * A resolver takes a host name and service name and turns them into
	 * a list of endpoints.
	 * The list of endpoints is returned using an object of type ip::tcp::resolver::results_type
	 */

class Client
{
public:
	Client(boost::asio::io_context& io_, const std::string& host_name, const std::string& service_name)
		: io(io_), socket(io)
	{
		tcp::resolver resolver(io);
		endpoints = resolver.resolve(host_name, service_name);		
	}

	void Connect()
	{
		try
		{
			//asio::connect() try each endpoint(IPv4 and IPv6) from range to connect
			boost::asio::connect(socket, endpoints);
			while(true)
			{
				std::array<char, 128> buffer{};
				boost::system::error_code error;
		/*
		* std::array to hold received data. asio::buffer() convert array
		* into mutable_buffer. Function automatically determines the size of
		* the array to help prevent buffer overruns.
		*/
				socket.read_some(boost::asio::buffer(buffer), error);

				if(error == boost::asio::error::eof)
				{
					break;
				}
				else if(error)
				{
					throw boost::system::system_error(error);
				}
				std::cout.write(buffer.data(), 128);
			}
		}
		catch(std::exception& ex)
		{
			std::cout << ex.what() << std::endl;
		}
	}

private:
	boost::asio::io_context& io;
	tcp::resolver::results_type endpoints;
	tcp::socket socket;
};