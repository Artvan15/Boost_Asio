#pragma once


#include <iostream>
#include <boost/asio.hpp>

//class httpConnector
//{
//public:
//	httpConnector(boost::asio::io_context& io, const std::string ip, unsigned port)
//		: socket(io)
//	{
//		try
//		{
//			boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(ip, error), port);
//			socket.connect(endpoint, error);
//
//			if (!error)
//			{
//				std::cout << "Connected to ip " << ip << " on port " << port << std::endl;
//			}
//			else
//			{
//				std::cerr << error.message() << std::endl;
//			}
//		}
//		catch(std::exception& ex)
//		{
//			std::cerr << ex.what() << std::endl;
//		}
//		
//	}
//
//	void GetRespond()
//	{
//		if(socket.is_open())
//		{
//			std::string sRequest =
//				"GET /index.html HTTP/1.1\r\n"
//				"Host: example.com\r\n"
//				"Connection: close\r\n\r\n";
//
//			socket.write_some(boost::asio::buffer(sRequest.data(), sRequest.size()),
//				error);
//
//
//			/*size_t b_available = socket.available();
//			std::string sRespond(b_available, ' ');
//			socket.read_some(boost::asio::buffer(sRespond, sRespond.size()), er);
//			std::cout << sRespond << std::endl;*/
//
//			buffer.assign(1 * 1024, ' ');
//			GrabSomeData();
//
//			using namespace std::chrono_literals;
//			std::this_thread::sleep_for(2000ms);
//		}
//	}
//
//private:
//	void GrabSomeData()
//	{
//		socket.async_read_some(boost::asio::buffer(buffer),
//			[this](const boost::system::error_code& er, size_t bytes_transferred)
//		{
//			/*if(er == boost::asio::error::eof)
//			{
//				return;
//			}*/
//			if (bytes_transferred == 0)
//			{
//				return;
//			}
//			std::cout << "Successfully read " << bytes_transferred << std::endl;
//
//			if(bytes_transferred != buffer.size())
//			{
//				buffer.resize(bytes_transferred);
//			}
//
//			std::cout << buffer << std::endl << std::endl <<
//				"Buffer size: " << buffer.size() << std::endl << std::endl;
//
//			buffer.assign(buffer.size(), ' ');
//			GrabSomeData();
//		});
//	}
//
//	std::string buffer;
//	boost::asio::ip::tcp::socket socket;
//	boost::system::error_code error;
//};

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
