//modified from original source: https://en.cppreference.com/w/cpp/memory/unique_ptr
#include <cassert>
#include <cstdio>
#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>

using std::cout, std::unique_ptr, std::FILE, std::fclose, std::move, std::data, std::make_unique, std::ofstream, std::fopen, std::fgetc, std::getline, std::string;

//runtime polymorphic classes:
struct Base
{
	virtual ~Base() = default;
	
	virtual void funct()
	{
		cout << "Base::funct\n";
	}
};

struct Derived : Base
{
	Derived()
	{
		cout << "Derived::constructor\n";
	}
	
	~Derived()
	{
		cout << "Derived::destructor\n";
	}
	
	void funct() override
	{
		cout << "Derived::funct\n";
	}
};

//function that consumes a unique pointer, it passes the pointer's value into
//the function and can handle both regular 'values' and 'Lvalues'
unique_ptr<Derived> pass(std::unique_ptr<Derived> ptr)
{
	cout << "\npassing ptr\n\n";
	ptr->funct();
	return ptr;
}

//Creates linked lists made with unique pointers:
struct uniquePtrList
{
	struct Index
	{
		int value;
		unique_ptr<Index> next;
	};

	unique_ptr<Index> top;

	~uniquePtrList()
	{
		while(top)//destroy list from top to bottom instead of recursively like default destructor does
		{
			top = move(top->next);
		}
	}

	void push(int value)
	{
		top = unique_ptr<Index>(new Index{value, move(top)});
	}
};

//file closer:
void closeFile(FILE* fptr)
{
	fclose(fptr);
	cout << "\nfile closed!\n";
}

int main()
{
	cout << "\nownership semantics:\n\n";
	{
		//create unique ptr of Derived class
		unique_ptr<Derived> ptr = make_unique<Derived>();
		//create a unique ptr aka 'passer' to 'pass' the pointer into the funct return value
		unique_ptr<Derived> passer = pass(move(ptr));
		//ptr has been moved, assert that it is nullptr
		assert(!ptr);
	}
	
	cout << "\nruntime polymorphism:\n\n";
	{
		//base ptr points to the Derived resource
		unique_ptr<Base> ptr = make_unique<Derived>();
		//dynamically dispatch a function like you normally would
		ptr->funct();
	}

	cout << "\nfile closer w/ unique_ptr:\n";
	const char* filename = "imaginaryfile.ext";
	ofstream(filename) << "\ncontent added to the file\n\n";
	{
		using unique_fptr = unique_ptr<FILE, decltype(&closeFile)>;
		FILE* fptr = fopen(filename, "r");
		unique_fptr ufptr(fptr, &closeFile);
	}

	cout << "\nlambda deleter with exception safety:\n\n";
	try
	{
		unique_ptr<Derived, void(*)(Derived*)> lambda(new Derived, [](Derived* dptr)
		{
			cout << "\ndestroying lambda from custom deleter...\n\n";
			delete dptr;
		});
		throw string("\nthis is an exception thrown in string format\n");//you can use std::runtime_error too
	}
	catch(string ex)
	{
		cout << "\nException:" << ex;
	}

	cout << "\nunique_ptr arrays:\n\n";
	{
		unique_ptr<Derived[]> uptrArr(new Derived[3]);
	}

	cout << "\nunique_ptr linked list:\n";
	{
		uniquePtrList peers;
		int peer = 0;
		while (peer <= 10)
		{
			peers.push(peer);
			string peerSize = std::to_string(peer);
			peer++;
			cout << "\nNumber of peers in list: " << peerSize;
		}
		cout <<'\n';
	}
}

