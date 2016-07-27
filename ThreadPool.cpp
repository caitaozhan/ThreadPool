#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t threads): m_stop(false)
{
	for (size_t i = 0; i < threads; ++i)
	{
		m_workers.emplace_back(
			[this]                            // capture this thread pool instance
			{
				for (;;)                      // avoid spurious false wakeups
				{
					function<void()> task;    // a container that holds function objects
					{// critical area
						unique_lock<mutex> lock(this->m_queueMutex);
						this->m_condition.wait(lock, [this] {return this->m_stop || !this->m_tasks.empty(); });  // blocks if pred returns false
						
						if (this->m_stop && this->m_tasks.empty())
							return;

						task = move(this->m_tasks.front());
						this->m_tasks.pop();
					}
					task();                   // run the task
				}
			}
		);
	}
}

ThreadPool::~ThreadPool()
{
	{// critical area
		unique_lock<mutex> lock(m_queueMutex);
		m_stop = true;
	}

	m_condition.notify_all();

	for (thread & worker : m_workers)
		worker.join();
}
