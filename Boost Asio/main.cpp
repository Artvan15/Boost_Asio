
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>


/*
 * Callback handlers will only be called from threads
 * that are currently calling run()
 *
 * We need pool of threads and method of synchronisation when handlers might
 * be accessing a shared, thread-unsafe resource
 */


class Printer
{
	using error_code = boost::system::error_code;
public:
	Printer(boost::asio::io_context& io)
		: strand_(boost::asio::make_strand(io)),
	timer1_(io, boost::asio::chrono::seconds(1)),
	timer2_(io, boost::asio::chrono::seconds(1))
	{
		/*
		 * 'bind_executor()' function returns a new handler that
		 * automatically dispatches its contained handler through the
		 * strand object. By binding the handlers to the same strand,
		 * we ensuring that they cannot execute concurrently
		 */

		
		timer1_.async_wait(boost::asio::bind_executor(strand_,
			[this](const error_code&)
			{
				print1();
			}));

		timer2_.async_wait(boost::asio::bind_executor(strand_,
			[this](const error_code&)
			{
				print2();
			}));
	}

	void print1()
	{
		if(count_ < 10)
		{
			std::cout << "Timer1: " << count_++ << std::endl;

			timer1_.expires_at(timer1_.expiry() + boost::asio::chrono::seconds(1));
			timer1_.async_wait(boost::asio::bind_executor(strand_,
				[this](const error_code&)
				{
					print1();
				}));
		}
	}

	void print2()
	{
		if(count_ < 10)
		{
			std::cout << "Timer2: " << count_++ << std::endl;

			timer2_.expires_at(timer2_.expiry() + boost::asio::chrono::seconds(1));
			timer2_.async_wait(boost::asio::bind_executor(strand_,
				[this](const error_code&)
				{
					print2();
				}));
		}
	}

	~Printer()
	{
		std::cout << count_ << '\n';
	}

/// <summary>
///				In a multithreaded program, the handlers for asynchronous
///				operations should be synchronised if they access shared
///				resources
/// </summary>
	
private:

	/*
	 * The 'strand' class guarantees that, for those handlers that are
	 * dispatched through it, an executing handler will be allowed to
	 * complete before the next one is started.
	 * Other handlers can execute concurrently
	 */
	
	boost::asio::strand<boost::asio::io_context::executor_type> strand_;
	boost::asio::steady_timer timer1_,
		timer2_;
	int count_ = 0;
};


int main()
{
	boost::asio::io_context io;
	Printer printer(io);
	boost::thread t([&io]()
		{
			io.run();
		});

	/*
	 * 'run()' is called from two threads: the main thread and one additional.
	 * The background thread will not exit until all asynchronous operations
	 * have completed
	 */
	
	io.run();
	t.join();
}