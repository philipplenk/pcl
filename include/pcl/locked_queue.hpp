#ifndef PHIL_CONCURRENCY_LIBRARY_LOCKED_QUEUE_H
#define PHIL_CONCURRENCY_LIBRARY_LOCKED_QUEUE_H

#include <pcl/monitor.hpp>

#include <optional>
#include <queue>

namespace pcl
{

template<typename T, typename QUEUE_T=std::queue<T>>
class locked_queue
{
	public:
	void push(T new_value)
	{
		data([new_value=std::move(new_value)]
			(data_t &data, typename monitor<data_t>::signal_fun f) mutable
		{
			data.queue.push(std::move(new_value));
			f.signal(data.cond);
		});
	}
	
	T wait_and_pop()
	{
		return data([](data_t &data, typename monitor<data_t>::signal_fun f)
		{
			while(data.queue.empty())
				f.wait(data.cond);
				
			auto ret_val=std::move(data.queue.front());
			data.queue.pop();
			return ret_val;
		});
	}
	
	std::optional<T> try_pop()
	{
		return data([](data_t &data)
		{
			if(data.queue.empty())
				return std::optional<T>{};
			auto ret_val=std::move(data.queue.front());
			data.queue.pop();
			return std::optional<T>{std::move(ret_val)};
		});
	}
	
	bool empty() const
	{
		return data([](const data_t &data)
		{
			return data.queue.empty();
		});
	}
	
	private:
	struct data_t
	{
		QUEUE_T queue;
		typename monitor<data_t>::condition_var cond;
	};
	monitor<data_t> data;
};

} //end namespace pcl

#endif
