#pragma once

#include "NetMessage.h"
#include "ThreadSafeQueue.h"
#include "ConnectionServer.h"
#include "CustomMsgTypes.h"
#include <boost/asio.hpp>
#include <set>




namespace net
{
	template<typename T>
	class Server
	{
	public:
		Server()
			: acceptor_(io_) {}

		~Server()
		{
			if (thread_.joinable())
				thread_.join();
		}

		//Set endpoint, start listen, launch handlers in other thread
		void Run(unsigned port);

		void Stop()
		{
			io_.stop();
			std::cout << "Server stopped\n";
		}


		//if socket isn't closed - send message
		//Otherwise - erase connection from queue and send client's id to other clients
		void MessageClient(std::weak_ptr<ConnectionServer<T>> con, const Message<T>& message);


		//Works the same as MessageClient, but for all in queue except ignored client
		void MessageAllClients(const Message<T>& message, std::shared_ptr<ConnectionServer<T>> ignore_client = nullptr);


		//working in loop - checking for incoming messages
		//Call OnMessage(message)
		void Update(size_t max_messages = ULONG_MAX);

	private:

		//ASYNC - async_accept for new connections
		//Create new connection and call OnClientConnected(connection)
		void Listen();


		//Insert connection in queue
		//Send approve message to connected client
		//Send inform message to other clients about connected client
		bool OnClientConnected(std::shared_ptr<ConnectionServer<T>> connection);


		//Send inform message to other clients about disconnected client
		void OnClientDisconnected(uint32_t id);


		//Handle income message
		void OnMessage(std::shared_ptr<MessageWithConnection<T>>&& income_message);
		

	private:
		boost::asio::io_context io_;
		boost::asio::ip::tcp::acceptor acceptor_;
		std::condition_variable condition_variable_;
		std::set<std::shared_ptr<ConnectionServer<T>>> connections_;
		ThreadSafeQueue<MessageWithConnection<T>> messages_in_;
		uint32_t new_connection_id = 0;
		std::thread thread_;
		std::mutex mutex_for_queue_;
	};




	
	/////////////////////////////// METHODS /////////////////////////
	
	template <typename T>
	void Server<T>::Run(unsigned port)
	{
		boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
		acceptor_.open(endpoint.protocol());
		acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		acceptor_.bind(endpoint);
		acceptor_.listen();
		Listen();
		std::cout << "Start listening on " << port << std::endl;
		thread_ = std::thread([this]() { io_.run(); });
	}


	template <typename T>
	void Server<T>::MessageClient(std::weak_ptr<ConnectionServer<T>> con, const Message<T>& message)
	{
		if (!con.expired())
		{
			if (auto connection = con.lock(); connection->IsConnected())
			{
				connection->Send(message);
			}
			else
			{
				//TODO: Try to erase messages from deleted connection
				uint32_t id = connection->GetId();
				connections_.erase(connection);
				OnClientDisconnected(id);
			}
		}
	}


	template <typename T>
	void Server<T>::MessageAllClients(const Message<T>& message, std::shared_ptr<ConnectionServer<T>> ignore_client)
	{
		for (auto con = connections_.begin(), end = connections_.end(); con != end;)
		{
			if (*con && (*con)->IsConnected())
			{
				if (*con != ignore_client)
				{
					(*con)->Send(message);
				}
			}
			else
			{
				uint32_t id = (*con)->GetId();
				connections_.erase(con++);
				OnClientDisconnected(id);
				continue;
			}
			++con;
		}
	}

	
	template <typename T>
	void Server<T>::Update(size_t max_messages)
	{
		//wait for insertion in queue
		std::unique_lock<std::mutex> unique_lock(mutex_for_queue_);
		condition_variable_.wait(unique_lock);

		for (size_t message_count = 0; message_count < max_messages &&
			!messages_in_.Empty(); ++message_count)
		{
			//grab front income message
			//handle the message
			OnMessage(messages_in_.PopFront());
		}
	}


	template <typename T>
	void Server<T>::Listen()
	{
		//socket is already bind to new-come connection
		acceptor_.async_accept([this](const boost::system::error_code& er, boost::asio::ip::tcp::socket socket)
			{
				if (!er)
				{
					std::cout << "Client connected: " << socket.remote_endpoint().address() <<
						" : " << socket.remote_endpoint().port() << std::endl;

					//create shared_ptr<ConnectionServer<T> and pass it to function
					OnClientConnected(ConnectionServer<T>::Create(
						io_, std::move(socket), messages_in_, new_connection_id++, condition_variable_));
				}
				else
				{
					std::cerr << er.message() << std::endl;
				}

				//continue listening for new connection
				Listen();

			});
	}


	template <typename T>
	bool Server<T>::OnClientConnected(std::shared_ptr<ConnectionServer<T>> connection)
	{
		//TODO: Verification

		connections_.insert(connection);
		std::cout << "[" << connection->GetId() << "] Connection Approved\n";

		//Message for connected client (without body)
		Message<CustomMsgTypes> message_for_client{ {CustomMsgTypes::ServerAccept} };
		MessageClient(connection, message_for_client);

		//Message for other clients
		Message<CustomMsgTypes> message_for_other_clients{ {CustomMsgTypes::MessageAllConnectedClient} };
		message_for_other_clients << connection->GetId();
		MessageAllClients(message_for_other_clients, connection);

		//Start reading from socket
		connection->ReadHeader();

		return true;
	}


	template <typename T>
	void Server<T>::OnClientDisconnected(uint32_t id)
	{
		std::cout << "Removing client [" << id << "]\n";
		Message<CustomMsgTypes> message_for_other_clients{ {CustomMsgTypes::MessageAllDisconnectedClient} };
		message_for_other_clients << id;
		MessageAllClients(message_for_other_clients);
	}


	template <typename T>
	void Server<T>::OnMessage(std::shared_ptr<MessageWithConnection<T>>&& income_message)
	{
		if (!income_message->ptr_connection.expired())
		{
			auto connection = income_message->ptr_connection.lock();
			switch (income_message->header.id)
			{
			case CustomMsgTypes::ServerPing:
			{
				std::cout << "[" << connection->GetId() << "]: Server Ping\n";
				MessageClient(connection, static_cast<Message<T>>(*income_message));
			}
			break;

			case CustomMsgTypes::MessageAll:
			{
				std::cout << "[" << connection->GetId() << "]: Message All\n";
				Message<CustomMsgTypes> message{ {CustomMsgTypes::MessageAll } };
				message << connection->GetId();
				MessageAllClients(message, connection);
			}
			break;
			}
		}
	}

}
