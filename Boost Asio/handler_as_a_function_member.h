#pragma once

#include <iostream>
#include <boost/asio.hpp>

namespace function_member
{
	/*
 * Handler must have error_code as first argument
 * To reset timer we sent reference into function
 */

	void print(const boost::system::error_code& ec,
		boost::asio::steady_timer& timer, int& counter)
	{
		if (counter < 5)
		{
			std::cout << counter << std::endl;
			++counter;

			//calculate new expiry time relative to the old
			timer.expires_at(timer.expiry() + boost::asio::chrono::seconds(1));

			/*
			 * steady_timer::async_wait() function expects a handler function
			 * (or function object) with the signature - void(const ::error_code&)
			 *
			 * Use lambda to create function object with additional params
			 */
			timer.async_wait(
				[&timer, &counter](const boost::system::error_code& ec)
				{
					print(ec, timer, counter);
				});
		}
	}

	class Printer
	{
	public:
		Printer(boost::asio::io_context& io)
			: timer_(io, boost::asio::chrono::seconds(1))
		{
			//set async_wait for timer
			timer_.async_wait([this](const boost::system::error_code& ec)
				{
					print();
				});
		}

		~Printer()
		{
			std::cout << counter_;
		}

		//member-function 'print' is a callback handler

		void print()
		{
			if (counter_ < 5)
			{
				std::cout << counter_++ << std::endl;

				timer_.expires_at(timer_.expiry() + boost::asio::chrono::seconds(1));

				timer_.async_wait([this](const boost::system::error_code& ec)
					{
						print();
					});
			}
		}
	private:
		boost::asio::steady_timer timer_;
		int counter_ = 0;
	};


	///////////////// main /////////////////
	/*boost::asio::io_context io;
	Printer printer(io);
	io.run();*/
	
}