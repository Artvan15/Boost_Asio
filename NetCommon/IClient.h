#pragma once
#include <string>


struct IClient
{
	virtual bool Connect(const std::string& host, unsigned port) = 0;
	virtual bool Disconnect() = 0;
	virtual bool IsConnected() const = 0;

	virtual ~IClient() = default;
};
