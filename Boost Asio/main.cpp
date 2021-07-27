
#include <iostream>
#include <boost/asio.hpp>


/*
 * Handler must have error_code as first argument
 * To reset timer we sent reference into function
 */

void print(const boost::system::error_code& ec,
	boost::asio::steady_timer& timer, int& counter)
{
	if(counter < 5)
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



int main()
{
	int counter = 0;
	boost::asio::io_context io;
	boost::asio::steady_timer timer(io, boost::asio::chrono::seconds(1));

	//first call of print function
	timer.async_wait(
		[&timer, &counter](const boost::system::error_code& ec)
		{
			print(ec, timer, counter);
		});

	io.run();

	std::cout << counter << std::endl;
	
}