#pragma once
#include <deque>
#include <iostream>
#include <mutex>
#include <string>


template<typename T>
class ThreadSafeQueue
{
public:
	ThreadSafeQueue() = default;
			
	ThreadSafeQueue(const ThreadSafeQueue& other)
	{
		std::lock_guard<std::mutex> lock_guard(other.mutex_);
		queue_ = other.queue_;
	}
	ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;
	ThreadSafeQueue(ThreadSafeQueue&&) = delete;
	ThreadSafeQueue& operator=(ThreadSafeQueue&&) = delete;
	~ThreadSafeQueue()
	{
		Clear();
	}
	
	void Clear();

	T& Front();
	
	const T& Front() const;
	
	T& Back();
	
	const T& Back() const;
	
	bool Empty() const;

	void PushFront(const T& value);
	
	void PushFront(T&& value);
	
	void PushBack(const T& value);
	
	void PushBack(T&& value);
	
	template<typename... Args>
	void EmplaceBack(Args&&... args);
	
	template<typename... Args>
	void EmplaceFront(Args&&... args);

	std::shared_ptr<T> PopBack();
	
	void PopBack(T& ref);

	std::shared_ptr<T> PopFront();
	
	void PopFront(T& value);

	
private:
	std::deque<T> queue_;
	mutable std::mutex mutex_;
};


template <typename T>
void ThreadSafeQueue<T>::Clear()
{
	std::lock_guard<std::mutex> lock_guard(mutex_);
	queue_.clear();
}

template <typename T>
T& ThreadSafeQueue<T>::Front()
{
	std::lock_guard<std::mutex> lock_guard(mutex_);
	return queue_.front();
}

template <typename T>
const T& ThreadSafeQueue<T>::Front() const
{
	std::lock_guard<std::mutex> lock_guard(mutex_);
	return queue_.front();
}

template <typename T>
T& ThreadSafeQueue<T>::Back()
{
	std::lock_guard<std::mutex> lock_guard(mutex_);
	return queue_.back();

}

template <typename T>
const T& ThreadSafeQueue<T>::Back() const
{
	std::lock_guard<std::mutex> lock_guard(mutex_);
	return queue_.back();
}

template <typename T>
bool ThreadSafeQueue<T>::Empty() const
{
	std::lock_guard<std::mutex> lock_guard(mutex_);
	return queue_.empty();
}

template <typename T>
void ThreadSafeQueue<T>::PushFront(const T& value)
{
	std::lock_guard<std::mutex> lock_guard(mutex_);
	queue_.push_front(value);
}

template <typename T>
void ThreadSafeQueue<T>::PushFront(T&& value)
{
	std::lock_guard<std::mutex> lock_guard(mutex_);
	queue_.push_front(value);
}

template <typename T>
void ThreadSafeQueue<T>::PushBack(const T& value)
{
	std::lock_guard<std::mutex> lock_guard(mutex_);
	queue_.push_back(value);
}

template <typename T>
void ThreadSafeQueue<T>::PushBack(T&& value)
{
	std::lock_guard<std::mutex> lock_guard(mutex_);
	queue_.push_back(value);
}

template <typename T>
template <typename ... Args>
void ThreadSafeQueue<T>::EmplaceBack(Args&&... args)
{
	std::lock_guard<std::mutex> lock_guard(mutex_);
	queue_.emplace_back(std::forward<Args>(args)...);
}

template <typename T>
template <typename ... Args>
void ThreadSafeQueue<T>::EmplaceFront(Args&&... args)
{
	std::lock_guard<std::mutex> lock_guard(mutex_);
	queue_.emplace_front(std::forward<Args>(args)...);
}

template <typename T>
std::shared_ptr<T> ThreadSafeQueue<T>::PopBack()
{
	std::lock_guard<std::mutex> lock_guard(mutex_);
	auto ptr = std::make_shared<T>(queue_.back());
	queue_.pop_back();
	return ptr;
}

template <typename T>
void ThreadSafeQueue<T>::PopBack(T& ref)
{
	std::lock_guard<std::mutex> lock_guard(mutex_);
	ref = queue_.back();
	queue_.pop_back();
}

template <typename T>
std::shared_ptr<T> ThreadSafeQueue<T>::PopFront()
{
	std::lock_guard<std::mutex> lock_guard(mutex_);
	auto ptr = std::make_shared<T>(std::move(queue_.front()));
	queue_.pop_front();
	return ptr;
}

template <typename T>
void ThreadSafeQueue<T>::PopFront(T& value)
{
	std::lock_guard<std::mutex> lock_guard(mutex_);
	value = queue_.front();
	queue_.pop_front();
}
