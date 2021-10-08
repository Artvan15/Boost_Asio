#pragma once
#include <cstdint>

enum class CustomMsgTypes : uint32_t
{
	ServerAccept,
	ServerDeny,
	ServerPing,
	MessageAllConnectedClient,
	MessageAllDisconnectedClient,
	MessageAll,
	ServerMessage,
};
