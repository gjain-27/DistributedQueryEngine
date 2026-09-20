#include "thread_pool.hpp"

ThreadPool::ThreadPool(size_t threadCount) : mStop(false) {
	for (size_t i = 0; i < threadCount; i++) {
		mWorkers.emplace_back([this]() {
			while (true) {
				std::function<void()> task;

				{
					std::unique_lock<std::mutex> lock(mQueueMutex);

					mCondition.wait(lock, [this]() { return mStop || !mTasks.empty(); });

					if (mStop && mTasks.empty()) return;

					task = std::move(mTasks.front());
					mTasks.pop();
				}

				task();
			}
		});
	}
}

ThreadPool::~ThreadPool() {
	{
		std::unique_lock<std::mutex> lock(mQueueMutex);
		mStop = true;
	}

	mCondition.notify_all();

	for (std::thread& worker : mWorkers) {
		worker.join();
	}
}