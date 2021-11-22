#pragma once

#include "ThreadSafeQueue.h"
#include "NetMessage.h"
#include <boost/asio.hpp>



namespace net
{
	//Base class for "ConnectionClient" and for "ConnectionServer"
	template<typename T, typename Derived>
	class Connection
	{			
	protected:
		Connection(boost::asio::io_context& io,
			boost::asio::ip::tcp::socket&& socket)
				: io_(io), socket_(std::move(socket)) {}
	public:

		//Close socket. Add this task to token queue in io
		void Disconnect();
		
		bool IsConnected() const
		{
			return socket_.is_open();
		}

		//Add message to output_message queue.
		//If message write loop is stopped - start a new one calling WriteHeader()
		void Send(const Message<T>& message);
		

		//ASYNC - Start async_read to temp_message.header
		//if there is no body - add msg to queue and start another read
		//Otherwise resize temp_message.body and call ReadBody()
		//If error occur - close the socket
		void ReadHeader();
		

		//ASYNC - if message_out isn't empty - start async_write of front message.header
		//if message is without body, popped it from queue and call WriteHeader()
		//Otherwise - call WriteBody()
		void WriteHeader();

	
	protected:

		//ASYNC - start async_read to temp_message.body
		//call AddMessageToIncomeQueue via this pointer of derived class
		void ReadBody();


		//ASYNC - start async_write of front message.body
		//popped front message and call WriteHeader
		void WriteBody();

		//called through derived_ pointer to child
		void AddMessageToIncomeQueue(const Message<T>& message);
		
	protected:
		boost::system::error_code er;
		boost::asio::io_context& io_;
		boost::asio::ip::tcp::socket socket_;
		
		//temporary income message
		Message<T> temp_message;
		
		//client change queue of outcome message through pointer in
		ThreadSafeQueue<Message<T>> message_out_;			
	};




	template<typename T, typename Derived>
	void Connection<T, Derived>::Disconnect()
	{
		if (IsConnected())
		{
			boost::asio::post(io_,
				[this]()
				{
					socket_.close();
				});
		}
	}

	template<typename T, typename Derived>
	void Connection<T, Derived>::Send(const Message<T>& message)
	{
		//add this function to token queue (in other thread)
		boost::asio::post(io_,
			[this, message]()
			{
				bool WritingLoopStop = message_out_.Empty();
				message_out_.PushBack(message);

				//if writing loop stop, start a new loop
				if (WritingLoopStop)
				{
					WriteHeader();
				}
			});
	}

	template<typename T, typename Derived>
	void Connection<T, Derived>::ReadHeader()
	{
		boost::asio::async_read(socket_, boost::asio::buffer(&temp_message.header, sizeof(MessageHeader<T>)),
			[this](const boost::system::error_code& er, size_t bytes_transferred)
			{
				if (!er)
				{
					if (temp_message.header.size > 0)
					{
						temp_message.body.resize(temp_message.header.size);
						ReadBody();
					}
					else
					{
						//message is complete without a header
						AddMessageToIncomeQueue(temp_message);
						ReadHeader();
					}
				}
				else
				{
					std::cerr << "Read Header fail\t" << er.message() << std::endl;
					socket_.close();
				}
			});
	}

	template<typename T, typename Derived>
	void Connection<T, Derived>::WriteHeader()
	{
		if (!message_out_.Empty())
		{
			boost::asio::async_write(socket_, boost::asio::buffer(&message_out_.Front().header, sizeof(MessageHeader<T>)),
				[this](const boost::system::error_code& er, size_t bytes_transferred)
				{
					if (!er)
					{
						if (message_out_.Front().header.size > 0)
						{
							WriteBody();
						}
						else
						{
							//Message is complete without body
							message_out_.PopFront();
							WriteHeader();
						}
					}
					else
					{
						std::cerr << "Write Header fail\t" << er.message() << std::endl;
						socket_.close();
					}
				});

		}
	}


	template<typename T, typename Derived>
	void Connection<T, Derived>::ReadBody()
	{
		boost::asio::async_read(socket_, boost::asio::buffer(temp_message.body.data(), temp_message.header.size),
			[this](const boost::system::error_code& er, size_t bytes_transferred)
			{
				if (!er)
				{
					//send complete message to derived class (client or server)
					AddMessageToIncomeQueue(temp_message);
					ReadHeader();
				}
				else
				{
					std::cerr << "Read body fail\t" << er.message() << std::endl;
					socket_.close();
				}
			});
	}


	template<typename T, typename Derived>
	void Connection<T, Derived>::WriteBody()
	{
		boost::asio::async_write(socket_, boost::asio::buffer(message_out_.Front().body.data(), message_out_.Front().header.size),
			[this](const boost::system::error_code& er, size_t bytes_transferred)
			{
				if (!er)
				{
					message_out_.PopFront();
					WriteHeader();
				}
				else
				{
					std::cerr << "Write header fail\t" << er.message() << std::endl;
					socket_.close();
				}
			});
	}

	//static polymorphism
	template <typename T, typename Derived>
	void Connection<T, Derived>::AddMessageToIncomeQueue(const Message<T>& message)
	{
		static_cast<Derived*>(this)->AddMessageToIncomeQueue(message);
	}
}
