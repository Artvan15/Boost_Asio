
#include <iostream>
#include <thread>
#include <chrono>
#include <boost/asio.hpp>
#include "httpConnector.h"

void runClient()
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


class Printer
{
public:
	Printer(boost::asio::io_context& io)
		: io_(io),
		timer1_(io, std::chrono::seconds(1)),
		timer2_(io, std::chrono::seconds(3)),
		strand_(boost::asio::make_strand(io))
	{
		timer1_.async_wait(boost::asio::bind_executor(strand_, [this](const boost::system::error_code& er)
			{
				std::cout << "Adding 1 to " << counter++ << '\n';
				print1();
			}));

		timer2_.async_wait(boost::asio::bind_executor(strand_, [this](const boost::system::error_code& er)
			{
				std::cout << "Adding 1 to " << counter << '\n';
				counter += 1;
				print2();
			}));

	}

	void print1()
	{
		timer1_.expires_after(std::chrono::milliseconds(100));
		timer1_.async_wait(boost::asio::bind_executor(strand_, [this](const boost::system::error_code& er)
			{
				if (counter > 20)
				{
					return;
				}
				std::cout << "Adding 1 to " << counter++ << '\n';
				print1();
			}));
	}
	void print2()
	{
		timer2_.expires_after(std::chrono::milliseconds(100));
		timer2_.async_wait(boost::asio::bind_executor(strand_, [this](const boost::system::error_code& er)
			{
				if (counter > 20)
				{
					return;
				}
				std::cout << "Adding 1 to " << counter << '\n';
				counter += 1;
				print2();
			}));
	}

private:
	boost::asio::io_context& io_;
	boost::asio::steady_timer timer1_, timer2_;
	size_t counter = 0;
	boost::asio::strand<boost::asio::io_context::executor_type> strand_;
};


int main()
{
	boost::asio::io_context io;
	Printer printer(io);

	std::thread thread{ [&io]() { io.run(); } };
	std::thread thread2{ [&io]() { io.run(); } };

	io.run();
	thread.join();
	thread2.join();
}