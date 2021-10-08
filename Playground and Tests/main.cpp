
#include <iostream>
#include <thread>

#include <boost/asio.hpp>
#include "httpConnector.h"



int main()
{
	
	boost::asio::io_context io;
	HttpConnector connector(io, "51.38.81.49", 80);

	connector.ProcessRequest();
	std::thread t([&io]() { io.run(); });

	std::cout << "Main thread work\n";

	if (t.joinable())
	{
		t.join();
	}
}