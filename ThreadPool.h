#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <future>
#include <thread>
#include <mutex>
#include <memory>
#include <utility>
#include <stdexcept>
#include <functional>
#include <condition_variable>
using namespace std;

class ThreadPool
{
public:
	ThreadPool(size_t n);

	template<typename F, typename ...Args>
	auto enqueue(F && f, Args && ... args) -> future<typename result_of<F(Args ...)>::type>;  // add threads to the thread pool

	~ThreadPool();
private:
	vector<thread> m_workers;               // keep track of threads so that can use join()
	queue<function<void()>> m_tasks;        // a queue of tasks

	mutex m_queueMutex;                     // provide exclusive ownership
	condition_variable m_condition;         // an object able to block the calling thread until notified to resume

	bool m_stop;
};

// add a new thread to the thread pool
template<typename F, typename ...Args>
inline auto ThreadPool::enqueue(F && f, Args && ...args) -> future<typename result_of<F(Args ...)>::type>
{
	using return_type = typename result_of<F(Args...)>::type;  // result_of: duduce the return type

	auto task = make_shared<packaged_task<return_type()>>(
		bind(forward<F>(f), forward<Args>(args)...)
		);

	future<return_type> result = task->get_future();

	{// critical area
		unique_lock<mutex> lock(m_queueMutex);

		if (m_stop)
		{
			throw runtime_error("enqueue on stopped ThreadPool");
		}

		m_tasks.emplace([task] {(*task)(); });  // put the thread in the queue
	}

	m_condition.notify_one();
	return result;
}
