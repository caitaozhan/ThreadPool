#pragma once
#include <future>
#include <thread>
using namespace std;

class ThreadPool
{
public:
	ThreadPool(size_t n);

	template<typename F, typename ...Args>
	auto enqueue(F && f, Args && ... args) -> future<typename result_of<F(Args ...)>::type>;

	~ThreadPool();
private:

};

template<typename F, typename ...Args>
inline auto ThreadPool::enqueue(F && f, Args && ...args) -> future<typename result_of<F(Args ...)>::type>
{
	return future<typename result_of<F(Args ...)>::type>();
}
