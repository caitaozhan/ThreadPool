#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t threads): m_stop(false)
{
	for (size_t i = 0; i < threads; ++i)
	{
		m_workers.emplace_back(               // 注意：下面的这个lambda会一直执行，但是“不属于”ThreadPool构造函数里面的，比如lambda执行的时候，变量 i 不存在
			[this]                            // capture this thread pool instance
			{
				for (;;)                      // avoid spurious false wakeups
				{
					function<void()> task;    // wraps a function object
					{// critical area
						unique_lock<mutex> lock(this->m_queueMutex);
						// blocks this thread if pred returns false, i.e. thread pool do not stop and there is no task(empty() == true)
						this->m_condition.wait(lock, [this] {return this->m_stop || !this->m_tasks.empty(); });  
						
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
