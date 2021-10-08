#pragma once

#include "Connection.h"

namespace net
{
	template<typename T>
	class ConnectionClient : public Connection<T>
	{
	protected:
		//Sends io, new created socket connected to io,
		//pointer to this, so that base class can can call method in child class
		ConnectionClient(boost::asio::io_context& io,
			ThreadSafeQueue<Message<T>>& message_in)
			: Connection<T>(io, boost::asio::ip::tcp::socket(io), this),
				message_in_(message_in) {}

	public:
		static std::unique_ptr<ConnectionClient<T>> Create(
			boost::asio::io_context& io,
			ThreadSafeQueue<Message<T>>& message_in)
		{
			return std::unique_ptr<ConnectionClient<T>>(new ConnectionClient<T>(io, message_in));
		}

		//ASYNC - asynchronously connect and start reading header
		void ConnectToServer(const std::string& host, unsigned port);


	private:
		//called by "Connection" though this pointer that was passed in constructor
		void AddMessageToIncomeQueue(const Message<T>& message) override
		{						
			message_in_.EmplaceBack(message);
		}

	private:
		ThreadSafeQueue<Message<T>>& message_in_;
	};



	
	template<typename T>
	void ConnectionClient<T>::ConnectToServer(const std::string& host, unsigned port)
	{
		boost::asio::ip::tcp::resolver resolver(this->io_);
		auto endpoints = resolver.resolve(host, std::to_string(port));
		boost::asio::async_connect(this->socket_, endpoints,
			[this](const boost::system::error_code& er,
				const boost::asio::ip::tcp::endpoint& endpoint)
			{
				if (!er)
				{
					//connection is complete. Start listening for income messages
					this->ReadHeader();
				}
				else
				{
					std::cerr << "Connection failed\t" << er.message();
				}
			});
	}
}