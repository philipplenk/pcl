#ifndef PHIL_CONCURRENCY_LIBRARY_MONITOR_H
#define PHIL_CONCURRENCY_LIBRARY_MONITOR_H

#include <condition_variable>
#include <mutex>
#include <thread>
#include <type_traits>

namespace pcl
{

template <typename T>
class monitor
{
	public:
	class signal_fun;
	class condition_var
	{
		std::condition_variable cond;
		bool signalled=false;
		friend class signal_fun;
		
		void wait(std::unique_lock<std::mutex>& l)
		{
			cond.wait(l,[&](){return signalled;});
			signalled=false;
		}
		
		void notify_one()
		{
			signalled=true;
			cond.notify_one();
		}
	};
	
	class signal_fun
	{
		public:
		friend class monitor;
		
		void signal(condition_var &var)
		{
			var.notify_one();
		}
		
		void wait(condition_var &var)
		{
			var.wait(l);
		}
		
		void yield()
		{
			l.unlock();
			std::this_thread::yield();
			l.lock();
		}
		
		private:
		signal_fun(std::unique_lock<std::mutex> &l):
			l(l)
		{}
		
		std::unique_lock<std::mutex> &l;
	};
	
	template <typename ...ARG_T>
	explicit monitor(ARG_T&& ...args):
		data(std::forward<ARG_T>(args)...)
	{}
	
	template <typename FUN_T>
	auto operator()(FUN_T fun)
	{
		return call(*this,fun);
	}
		
	template <typename FUN_T>
	auto operator()(FUN_T fun) const
	{
		return call(*this,fun);
	}
	
	private:
	mutable std::mutex m;
	T data;
	
	template <typename deduced_const_self, typename FUN_T>
	static auto call(deduced_const_self& self, FUN_T fun)
	{
		using argument_type = std::conditional_t<std::is_const_v<deduced_const_self>,const T&, T&>;
		constexpr auto invocable_without_signal = std::is_invocable_v<FUN_T,argument_type>;
		constexpr auto invocable_with_signal = std::is_invocable_v<FUN_T,argument_type,signal_fun>;
		
		static_assert(invocable_without_signal || invocable_with_signal,
			"The callable passed to monitor must be invocable as fun(data,[signal_fun])");
		
		if constexpr(invocable_without_signal)
		{
			std::lock_guard<std::mutex> lock(self.m);
			return fun(self.data);
		}
		else if constexpr(invocable_with_signal)
		{
			std::unique_lock<std::mutex> lock(self.m);
			return fun(self.data,signal_fun{lock});
		}
	}
};

} //end namespace pcl

#endif
