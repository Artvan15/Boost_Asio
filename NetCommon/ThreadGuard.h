#pragma once

#include <thread>

class ThreadGuard
{
public:
	ThreadGuard(std::thread& thread)
		: thread_(thread) {}
	
	ThreadGuard(const ThreadGuard&) = delete;
	ThreadGuard(ThreadGuard&&) = delete;
	
	~ThreadGuard()
	{
		if (thread_.joinable())
			thread_.join();
	}

private:
	std::thread& thread_;
};