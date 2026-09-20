#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <type_traits>

class ThreadPool {
public:
	explicit ThreadPool(size_t threadCount = std::thread::hardware_concurrency());
	~ThreadPool();

	template<typename F, typename... Args>
	auto enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>;

private:
	std::vector<std::thread> mWorkers;
	std::queue<std::function<void()>> mTasks;

	std::mutex mQueueMutex;
	std::condition_variable mCondition;
	bool mStop;
};

template<typename F, typename... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
	using ReturnType = std::invoke_result_t<F, Args...>;

	auto task = std::make_shared<std::packaged_task<ReturnType()>>(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...)
	);

	std::future<ReturnType> result = task->get_future();

	{
		std::unique_lock<std::mutex> lock(mQueueMutex);

		if (mStop) throw std::runtime_error("enqueue called on stopped ThreadPool");

		mTasks.emplace([task]() { (*task)(); });
	}

	mCondition.notify_one();

	return result;
}