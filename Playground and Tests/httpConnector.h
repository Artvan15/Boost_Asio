#pragma once


#include <iostream>
#include <boost/asio.hpp>


class HttpConnector
{
	using tcp = boost::asio::ip::tcp;
public:
	HttpConnector(boost::asio::io_context& io, const std::string host, unsigned port)
		: endpoint(boost::asio::ip::make_address(host, er), port),
		socket(io),
		message(1 * 1024, ' ')
	{
		try
		{
			socket.connect(endpoint, er);
			if (!er)
			{
				std::cout << "Successfully connected to " << endpoint.address() <<
					" : " << endpoint.port() << std::endl;
			}
			else
			{
				std::cerr << er.message();
			}
			
		}
		catch (const std::exception& ex)
		{
			std::cerr << ex.what();
		}
	}


	void ProcessRequest()
	{
		if (socket.is_open())
		{

			std::string sRequest =
				"GET /index.html HTTP/1.1\r\n"
				"Host: example.com\r\n"
				"Connection: close\r\n\r\n";

			socket.async_write_some(boost::asio::buffer(sRequest.data(), sRequest.size()),
				[this](const boost::system::error_code& er, size_t b_size)
				{
					std::cout << "Sent " << b_size << std::endl;
					GetMessage();
				});
			std::cout << "Sending request...\n";
		}
		else
		{
			std::cerr << "Socket closed\n";
		}

	}

	

private:
	void GetMessage()
	{
		socket.async_read_some(boost::asio::buffer(message),
			[this](const boost::system::error_code& er, size_t b_size)
			{
				if (er == boost::asio::error::eof)
				{
					return;
				}
				else if (er)
				{
					std::cout << er.message();
					return;
				}

				std::cout << "Read " << b_size << std::endl;
				if(b_size != message.size())
				{
					message.resize(b_size);
				}
				std::cout.write(message.data(), message.size());
				std::cout << '\n' << '\n';
				message.assign(message.size(), ' ');
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				GetMessage();
			});

		
	}
	
	boost::system::error_code er;
	tcp::endpoint endpoint;
	tcp::socket socket;
	std::string message;
};

template<typename T>
class PosholNahui
{
	void awesomeC();
};

template <typename T>
void PosholNahui<T>::awesomeC()
{
	
}
