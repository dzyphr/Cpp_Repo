#include <coroutine>
#include <iostream>
#include <stdexcept>
#include <thread>

auto new_thread(std::jthread& out)
{
	struct awaitable
	{
		std::jthread* p_out;
		bool await_ready() { return false; }
		void await_suspend(std::coroutine_handle<> h)
		{
			std::jthread& out = *p_out;
			if (out.joinable())
				throw std::runtime_error("output thread parameter is not emptied");
			out = std::jthread([h] { h.resume(); });
			std::cout << "new thread ID:" << out.get_id() << '\n';
		}
		void await_resume() {std::cout << "resumed on a new thread\n" ;}
	};
	return awaitable{&out};
}

struct task
{
	struct promise_type
	{
		task get_return_object() { return {}; }
		std::suspend_never initial_suspend() { return {};}
		std::suspend_never final_suspend() noexcept { return {}; }
		void return_void() {}
		void unhandled_exception() {}
	};
};

task resume_on_new_thread(std::jthread& out)
{
	std::cout << "coroutines started on thread: " << std::this_thread::get_id() << '\n';
	co_await new_thread(out);
	std::cout << "Coroutine resumed on thread: " << std::this_thread::get_id() << '\n';
}

int main()
{
	std::jthread out;
	resume_on_new_thread(out);
}
