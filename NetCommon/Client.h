#pragma once

#include "IClient.h"
#include "ConnectionClient.h"
#include "ThreadSafeQueue.h"
#include "CustomMsgTypes.h"
#include <thread>

namespace net
{
	template<typename T>
	class Client : public IClient
	{
	public:
		Client() = default;

		~Client() override;
		
		bool Connect(const std::string& host, unsigned port) override;
		
		//Stop asynchronous tasks and delete current connection
		bool Disconnect() override;

		//Checks if socket is still open
		bool IsConnected() const override;

		//Sends a timeNow to server
		void PingServer();

		//Sends message to all client (body is empty)
		void MessageAll();

		ThreadSafeQueue<Message<T>>& GetMessageInQueue()
		{
			return message_in_;
		}
	
	private:
		std::unique_ptr<ConnectionClient<T>> connection_ptr;
		ThreadSafeQueue<Message<T>> message_in_;
		boost::asio::io_context io_;
		std::thread thread_;	
	};





	
	template<typename T>
	Client<T>::~Client()
	{
		//get an opportunity for client to end io tasks
		if (thread_.joinable())
			thread_.join();

		if (connection_ptr)
			connection_ptr->Disconnect();
	}
	
	template<typename T>
	bool Client<T>::Connect(const std::string& host, unsigned port)
	{
		//Client has unique owning of ConnectionClient object
		connection_ptr = ConnectionClient<T>::Create(io_, message_in_);
		connection_ptr->ConnectToServer(host, port);

		//start all handlers and tokens in other thread. Context already has a work - async_connect
		thread_ = std::thread([this]() { io_.run(); });
		return true;
	}

	template <typename T>
	bool Client<T>::Disconnect()
	{
		if (connection_ptr)
			connection_ptr->Disconnect();

		io_.stop();
		connection_ptr = nullptr;
		return true;
	}

	template <typename T>
	bool Client<T>::IsConnected() const
	{
		if (connection_ptr)
			return connection_ptr->IsConnected();
		return false;
	}

	
	template <typename T>
	void Client<T>::PingServer()
	{
		Message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::ServerPing;

		std::chrono::system_clock::time_point timeNow =
			std::chrono::system_clock::now();

		msg << timeNow;

		connection_ptr->Send(msg);
	}

	template <typename T>
	void Client<T>::MessageAll()
	{
		Message<CustomMsgTypes> msg{ CustomMsgTypes::MessageAll };
		connection_ptr->Send(msg);
	}

	
}
