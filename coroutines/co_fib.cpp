#include <coroutine>
#include <exception>
#include <iostream>

template<typename T>
struct co_fib//this class name is completely customize, remember to rename its re-uses properly if so
{
	struct promise_type;
	using handle_type = std::coroutine_handle<promise_type>;

	struct promise_type
	{
		T value_;
		std::exception_ptr exception_;

		co_fib get_return_object()
		{
			return co_fib(handle_type::from_promise(*this));
		}
		std::suspend_always initial_suspend() {return{};}
		std::suspend_always final_suspend() noexcept {return{};}
		void unhandled_exception() {exception_ = std::current_exception();}
		template<std::convertible_to<T> From> std::suspend_always yield_value(From &&from)
		{
			value_ = std::forward<From>(from);
			return{};
		}
		void return_void() {}
	};

	handle_type co_fib_handle;

	co_fib(handle_type h) : co_fib_handle(h) {}
	~co_fib() {co_fib_handle.destroy();}
	explicit operator bool()
	{
		fill();//this executes or resumes coroutine until the next co_yield or until the coroutine falls off the end
		/*falling off the end is allowed when the function returns void or the coroutine completes with void, but if you do not 
		co_return a value it will become undefined behavior(the promises return value wont be called before reaching 
		final_suspend)
		excerpt from https://devblogs.microsoft.com/oldnewthing/20220718-00/?p=106870 documenting undefined behaviors
		For PPL tasks, the promise implementation just returns a default-constructed result; For hat-types, 
		that means a null pointer. For C++/WinRT asynchronous operations, the promise implementation returns 
		an empty result: For value types, you get a default-constructed value type; for reference types, you get a null pointer
		//
		result is stored in the promise to allow operator to grab it without executing coroutine*/
		return !co_fib_handle.done();
	}
	T operator()()
	{
		fill();
		full_ = false;
		return std::move(co_fib_handle.promise().value_);
	}
private: 
	bool full_  = false;
	void fill() 
	{
		if (!full_)
		{
			co_fib_handle();
			if (co_fib_handle.promise().exception_)
				std::rethrow_exception(co_fib_handle.promise().exception_);
			full_ = true;
		}
	}
};

co_fib<uint64_t>
fibonacci_sequence(unsigned n)
{
	if (n==0)
		co_return;

	if (n>94)
		throw std::runtime_error("Fib sequence too large causing overflow");

	co_yield 0;

	if (n==2)
		co_return;

	co_yield 1;

	uint64_t first = 0;
	uint64_t second = 1;

	for (unsigned i = 2; i < n; i++)
	{
		uint64_t ans = first + second;
		co_yield ans;
		first = second;
		second = ans;
	}
}

int main()
{
	try
	{
		auto gen = fibonacci_sequence(94);

		for (int k=0; gen; k++)
		{
			std::cout << "fib ("<< k << ") = " << gen() << 'n';
		}
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Exception: " << ex.what() << '\n';
	}
	catch (...)
	{
		 std::cerr << "Unknown exception.\n";
	}
}
