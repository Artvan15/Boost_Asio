#pragma once

#include <iostream>
#include <boost/asio.hpp>

namespace synch_asynch
{

	void func(const boost::system::error_code&)
	{
		std::cout << "Hello\n";
	}

	void func2(const boost::system::error_code&)
	{
		std::cout << "Hello22\n";
	}

	void test()
	{
		/*
		 * All programs that use asio need to have at least one I/O execution context,
		 * such as an io_context or thread_pool object.
		 * An I/O execution context provides access to I/O functionality.
		 *
		 *  the call to steady_timer::wait() will not return until the timer has expired,
		 *  5 seconds after it was created
		 */
		boost::asio::io_context io;
		boost::asio::steady_timer t(io, boost::asio::chrono::seconds(5));
		t.wait();
		std::cout << "Hello, Boost!\n";


		t.expires_after(boost::asio::chrono::seconds(20));
		t.async_wait(&func);


		/*
		 * Using asio's asynchronous functionality means having a callback function
		 * that will be called when an asynchronous operation completes
		 *
		 *  we pass the func callback handler that was defined above
		 *
		 *  timer starts right after ::async_wait was called
		 *
		 *  The asio library provides a guarantee
		 *  that callback handlers will only be called from threads
		 *  that are currently calling io_context::run(). And if io::context::run() is running
		 *
		 *  The io_context::run() function will also continue to run
		 *  while there is still "work" to do. io_context::run() will only finish if all handlers complete their work.
		 */

		boost::asio::steady_timer t2(io, boost::asio::chrono::seconds(6));
		t2.async_wait(&func2);
		boost::asio::steady_timer t3(io, boost::asio::chrono::seconds(15));
		t3.wait();
		std::cout << "main\n";
		io.run();
	}


	/////////////////// main //////////////////////
	/*int counter = 0;
	boost::asio::steady_timer timer(io, boost::asio::chrono::seconds(1));

	//first call of print function
	timer.async_wait(
		[&timer, &counter](const boost::system::error_code& ec)
		{
			print(ec, timer, counter);
		});

	io.run();

	std::cout << counter << std::endl;
	*/

}