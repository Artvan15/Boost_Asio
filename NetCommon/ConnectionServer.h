#pragma once

#include "Connection.h"
#include "MessageWithConnection.h"

namespace net
{
	template<typename T>
	class ConnectionServer : public Connection<T, ConnectionServer<T>>,
		public std::enable_shared_from_this<ConnectionServer<T>>
	{
		template<typename V>
		friend bool operator<(const ConnectionServer<V>&, const ConnectionServer<V>&);

		//friend access only for special Connection class
		friend class Connection<T, ConnectionServer<T>>;
		
	protected:
		ConnectionServer(boost::asio::io_context& io,
			boost::asio::ip::tcp::socket&& socket,
			ThreadSafeQueue<MessageWithConnection<T>>& message_in,
			uint32_t id,
			std::condition_variable& condition)
				: Connection<T, ConnectionServer<T>>(io, std::move(socket), this),
					message_in_(message_in), id_(id), condition_variable_(condition) {}

	public:
		static std::shared_ptr<ConnectionServer<T>> Create(
			boost::asio::io_context& io,
			boost::asio::ip::tcp::socket&& socket,
			ThreadSafeQueue<MessageWithConnection<T>>& message_in,
			uint32_t id,
			std::condition_variable& condition)
		{
			return std::shared_ptr<ConnectionServer<T>>(
				new ConnectionServer<T>(io, std::move(socket), message_in, id, condition));
		}
		
		uint32_t GetId() const
		{
			return id_;
		}

	private:
		void AddMessageToIncomeQueue(const Message<T>& message)
		{
			//Create MessageWithConnection and push into queue
			message_in_.EmplaceBack(message, this->shared_from_this());
			condition_variable_.notify_one();
		}

	private:
		//client stores queue of income message
		ThreadSafeQueue<MessageWithConnection<T>>& message_in_;
		uint32_t id_ = 0;
		std::condition_variable& condition_variable_;
	};




	
	template<typename V>
	bool operator<(const ConnectionServer<V>& lhs, const ConnectionServer<V>& rhs)
	{
		return lhs.id_ < rhs.id_;
	}
}