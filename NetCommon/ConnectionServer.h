#pragma once

#include "Connection.h"

namespace net
{
	template<typename T>
	class ConnectionServer : public Connection<T>, public std::enable_shared_from_this<ConnectionServer<T>>
	{
		template<typename V>
		friend bool operator<(const ConnectionServer<V>&, const ConnectionServer<V>&);
		
	protected:
		ConnectionServer(boost::asio::io_context& io,
			boost::asio::ip::tcp::socket&& socket,
			ThreadSafeQueue<MessageWithConnection<T>>& message_in,
			uint32_t id)
				: Connection<T>(io, std::move(socket), this),
					message_in_(message_in), id_(id) {}

	public:
		static std::shared_ptr<ConnectionServer<T>> Create(
			boost::asio::io_context& io,
			boost::asio::ip::tcp::socket&& socket,
			ThreadSafeQueue<MessageWithConnection<T>>& message_in,
			uint32_t id)
		{
			return std::shared_ptr<ConnectionServer<T>>(new ConnectionServer<T>(io, std::move(socket), message_in, id));
		}
		
		uint32_t GetId() const
		{
			return id_;
		}

	private:
		void AddMessageToIncomeQueue(const Message<T>& message) override
		{
			//Create MessageWithConnection and push into queue
			message_in_.EmplaceBack(message, this->shared_from_this());
		}

	private:
		//client stores queue of income message
		ThreadSafeQueue<MessageWithConnection<T>>& message_in_;
		uint32_t id_ = 0;
	};




	
	template<typename V>
	bool operator<(const ConnectionServer<V>& lhs, const ConnectionServer<V>& rhs)
	{
		return lhs.id_ < rhs.id_;
	}
}