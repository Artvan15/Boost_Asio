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


		t.expires_after(boost::asio::chrono::seconds(4));
		t.async_wait(&func);


		/*
		 * Using asio's asynchronous functionality means having a callback function
		 * that will be called when an asynchronous operation completes
		 *
		 *  we pass the func callback handler that was defined above
		 *
		 *  The asio library provides a guarantee
		 *  that callback handlers will only be called from threads
		 *  that are currently calling io_context::run()
		 *
		 *  The io_context::run() function will also continue to run
		 *  while there is still "work" to do
		 */

		boost::asio::steady_timer t2(io, boost::asio::chrono::seconds(6));
		t2.async_wait(&func2);
		io.run();
	}

}