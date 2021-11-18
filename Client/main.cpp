
#include <iostream>
#include <Client.h>
#include "CustomMsgTypes.h"


int main()
{
	setlocale(LC_ALL, "");
	net::Client<CustomMsgTypes> client;
	client.Connect("localhost", 80);


	bool key[3] = { false, false, false };
	bool old_key[3] = { false, false, false };

	bool bQuit = false;

	//Infinite loop checks if there is messages in income queue
	while(!bQuit)
	{
		//activewindow == consolewindow
		if(GetForegroundWindow() == GetConsoleWindow())
		{
			//key pressed or released
			key[0] = GetAsyncKeyState('1') & 0x8000;
			key[1] = GetAsyncKeyState('2') & 0x8000;
			key[2] = GetAsyncKeyState('3') & 0x8000;
		}

		if (key[0] && !old_key[0]) client.PingServer();
		if (key[1] && !old_key[1]) client.MessageAll();
		if (key[2] && !old_key[2]) bQuit = true;

		for (int i = 0; i != 3; ++i)
		{
			old_key[i] = key[i];
		}


		if(client.IsConnected())
		{
			if(!client.GetMessageInQueue().Empty())
			{
				auto msg = client.GetMessageInQueue().PopFront();

				switch(msg->header.id)
				{
				case CustomMsgTypes::ServerAccept:
					{
					std::cout << "Server accepted connection\n";
					}
					break;

					//when client sends message, server sends it back
				case CustomMsgTypes::ServerPing:
					{
						std::chrono::system_clock::time_point timeNow =
							std::chrono::system_clock::now();
						std::chrono::system_clock::time_point timeThen;
						*msg >> timeThen;

						std::cout << "Ping: " << std::chrono::duration<double>(timeNow - timeThen).count()
							<< std::endl;
					}
					break;
					
				case CustomMsgTypes::MessageAll:
					{
						uint32_t id;
						*msg >> id;
						std::cout << "Hello from [" << id << "]\n";
					}
					break;

				case CustomMsgTypes::MessageAllConnectedClient:
					{
						uint32_t id;
						*msg >> id;
						std::cout << "Client " << "[" << id << "] connected\n";
					}
					break;

				case CustomMsgTypes::MessageAllDisconnectedClient:
					{
						uint32_t id;
						*msg >> id;
						std::cout << "Client " << "[" << id << "] disconnected\n";
					}
					break;
				}
			}
		}
	}
}
