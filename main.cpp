#include <iostream>
#include <vector>
#include <string>
#include <future>
#include <thread>
#include <chrono>
#include "ThreadPool.h"
using namespace std;
using namespace chrono;

/*
	This thread pool can be seen as producer-consumer-like model.
	ThreadPool::m_tasks() is the critical section
	ThreadPool::m_workers() is the consumer, which can consume 4 tasks concurrently
	ThreadPool::enqueue() is the producer, which is called 32 times in main::for(...)
*/
int main()
{
	ThreadPool pool(4);                 // a pool that run 4 threads concurrently
	vector<future<string>> results;     // the results of threads
	for (int i = 0; i < 32; ++i)        // start 32 tasks
	{
		results.emplace_back(           // emplace a future<string> to vector
			pool.enqueue([i]            // once a thread in the pool is available, receive a new task
			{
				// every task's job is to print a string, sleep for 2 seconds, and return a string
				cout << string("thread " + to_string(i) + " is processing\n");
				this_thread::sleep_for(seconds(2));
				return string("---thread ") + to_string(i) + string(" is finished.---\n");
			})
		);
	}

	for (auto && result : results)
	{
		cout << result.get();
	}

	system("pause");
	return 0;
}
