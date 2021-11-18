#pragma once
#include "NetMessage.h"


namespace net
{
	template<typename T>
	class ConnectionServer;

	template<typename T>
	struct MessageWithConnection : public Message<T>
	{
		MessageWithConnection(const Message<T>& message, const std::shared_ptr<ConnectionServer<T>>& ptr)
			: Message<T>(message), ptr_connection(ptr) {}


		std::weak_ptr<ConnectionServer<T>> ptr_connection = nullptr;

		//conversation to Message<T>
		explicit operator Message<T>() const { return { this->header, this->body }; }

		template<typename V>
		friend std::ostream& operator<<(std::ostream& os, const MessageWithConnection<V>& message);
	};

	template<typename V>
	std::ostream& operator<<(std::ostream& os, const MessageWithConnection<V>& message)
	{
		return operator<<(os, message);
	}
}