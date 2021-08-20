
#include "Client.h"	

int main()
{
	boost::asio::io_context io;
	Client client(io, "localhost", "daytime");
	client.Connect();
}
