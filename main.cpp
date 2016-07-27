#include <iostream>
#include <vector>
#include <string>
#include <future>
#include <thread>
#include <chrono>
#include "ThreadPool.h"
using namespace std;
using namespace chrono;

int main()
{
	ThreadPool pool(4);                 // a pool that run 4 threads concurrently
	vector<future<string>> results;     // the results oT threads
	
	for (int i = 0; i < 8; ++i)         // start 8 tasks
	{
		results.emplace_back(
			pool.enqueue([i] 
			{
				cout << "hello " << i << endl;
				this_thread::sleep_for(chrono::seconds(1));
				cout << "world " << i << endl;
				return string("---thread ") + to_string(i) + string(" finished.---");
			})
		);
	}

	for (auto && result : results)
	{
		cout << result.get() << endl;
	}
	cout << endl;

	system("pause");
	return 0;
}
