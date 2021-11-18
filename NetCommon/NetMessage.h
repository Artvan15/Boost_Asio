#pragma once

#include <boost/asio.hpp>
#include <vector>
#include <iostream>

/*
 * A standard-layout class is a class that:

	has no non-static data members of type non-standard-layout class (or array of such types) or reference,
	has no virtual functions (10.3) and no virtual base classes (10.1),
	has the same access control (Clause 11) for all non-static data members,
	has no non-standard-layout base classes,
	either has no non-static data members in the most derived class and at most one base class with non-static data members, or has no base classes with non-static data members, and
	has no base classes of the same type as the first non-static data member.
 */


namespace net
{

	// Message header is in the start of all messages. User defined enum
	// by himself. Then he can assign values only from this enum to id 
	template<typename T>
	struct MessageHeader
	{
		T id{};
		uint32_t size = 0;
	};

	template<typename T>
	struct Message
	{
		MessageHeader<T> header;
		std::vector<uint8_t> body;

		/// Get information about Message
		friend std::ostream& operator<<(std::ostream& os, const Message& msg);

		/// <summary>
		/// Write data into message
		/// </summary>
		/// <typeparam name="DataType">Must be standard layout</typeparam>
		/// <param name="msg">Store data</param>
		/// <param name="data">Data to be written</param>
		/// <returns>Reference to a message</returns>
		template<typename V, typename DataType>
		friend Message<V>& operator<<(Message<V>& msg, const DataType& data);

		
		template<typename V, typename DataType>
		friend Message<V>& operator>>(Message<V>& msg, DataType& data);
	};


	template<typename T>
	std::ostream& operator<<(std::ostream& os, const Message<T>& msg)
	{
		os << "ID: " << static_cast<int>(msg.header.id) <<
			"\tSize: " << msg.header.size;
		return os;
	}

	
	template<typename T, typename DataType>
	Message<T>& operator<<(Message<T>& msg, const DataType& data)
	{
		//check if DataType is simple
		static_assert(std::is_standard_layout<DataType>::value, "Data type is too complex");
		size_t msg_size = msg.header.size;

		//resize vector and copy data in new allocated memory
		msg.body.resize(msg_size + sizeof(DataType));
		std::memcpy(msg.body.data() + msg_size, &data, sizeof(DataType));
		msg.header.size = msg.body.size();
		return msg;
	}

	template<typename T, typename DataType>
	Message<T>& operator>>(Message<T>& msg, DataType& data)
	{
		static_assert(std::is_standard_layout<DataType>::value, "Data type is too complex");

		//copy last DataType from vector
		size_t msg_without_last_data = msg.body.size() - sizeof(DataType);
		std::memcpy(&data, msg.body.data() + msg_without_last_data, sizeof(DataType));

		//adjust size of vector and size in header
		msg.body.resize(msg_without_last_data);
		msg.header.size = msg.body.size();
		return msg;
	}
		
	
}
