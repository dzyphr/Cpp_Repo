//modified from: https://en.cppreference.com/w/cpp/memory/shared_ptr
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>
#include <cstdarg>

using std::cout, std::shared_ptr, std::mutex, std::lock_guard, std::make_shared, std::ref;

struct SharedBase
{
	SharedBase()
	{
		cout << "SharedBase::SharedBase() constructor\n";
	}
	//non virtual destructor allowed
	~SharedBase()
	{
		cout << "SharedBase::~SharedBase() destructor\n";
	}
};

struct SharedDerived : public SharedBase
{
	SharedDerived()
	{
		cout << "SharedDerived::SharedDerived() constructor\n";
	}
	~SharedDerived()
        {
                cout << "SharedDerived::~SharedDerived() destructor\n";
        }
};

void spawn(shared_ptr<SharedBase> ptr)
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	shared_ptr<SharedBase> sharedBase = ptr;
	{
		static mutex mtx;
		lock_guard<mutex> lock(mtx);
		cout << "current pointer in thread:\n"
			<< "sharedBase value: " << sharedBase.get() << '\n' 
			<< "sharedBase use_count:" << sharedBase.use_count() << "\n\n";
	}
}

int joinThreads(int count, ...)//threads must be passed here by reference
{
	std::va_list thread_list;
	va_start(thread_list, count);
	int i = 0;
	while (i < count)
	{
		va_arg(thread_list, std::thread).join();
		i++;
	}
	cout << '\n' << std::to_string(i) << " threads joined\n\n";
	return i;
}

int main()
{
	shared_ptr<SharedBase> ptr = make_shared<SharedDerived>();
	cout << "\nshared pointer from derived class->base class\n"
		<< "values before threading:\n" 
		<< "ptr val: " << ptr.get()
		<< "ptr use_count: " << ptr.use_count() << "\n\n";
	ref(std::thread) thread1(spawn, ptr), thread2(spawn, ptr), thread3(spawn, ptr);
	ptr.reset();//release mains ownership of the ptr
	cout << "the shared pointers share ownership of each thread\n"
                << "instead of being owned by the main entrypoint:\n"
                << "ptr val: " << ptr.get() << '\n'
                << "ptr use_count: " << ptr.use_count() << "\n\n";
	//thread1.join(); thread2.join(); thread3.join();
	int i = 3; //number of threads
	int out = joinThreads(i, thread1, thread2, thread3);//using the output is optional but its there to stop a warning in the fn
							    //idea: use a ref to an array of threads in order to shorten the fn call
							    //this allows us to forego the use of variadic arguments which is verbose
	cout << std::to_string(out) << " threads joined, derived pointer deleted after last use\n\n";
}
