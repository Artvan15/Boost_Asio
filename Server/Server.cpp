
#include "Server.h"
#include "CustomMsgTypes.h"

using namespace net;




int main()
{
	setlocale(LC_ALL, "");
	Server<CustomMsgTypes> server;
	server.Run(80);

	while (true)
	{
		server.Update();
	}

}