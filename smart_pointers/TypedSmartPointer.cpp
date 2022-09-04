#include <iostream>
#include <sstream>
#include <string.h>
using std::cout, std::stringstream, std::to_string, std::string;
template <class Type>
class TypedSmartPointer
{
	Type* pointer;
public:
	//constructor
	explicit TypedSmartPointer(Type* constructor_pointer = NULL)
	{
		pointer = constructor_pointer;
	}
	//destructor
	~TypedSmartPointer()
	{
		delete(pointer);
	}
	//overload dereferencing operator to return smart pointer of Type
	Type& operator* ()
	{
		return *pointer;
	}
	//if T represents a type that needs to be accessed overload the arrow opperator
	//access the smart pointer like a regular pointer '->'
	Type* operator -> ()
	{
		return pointer;
	}
};

int main()
{
	TypedSmartPointer<const char*> anypointer(new const char*());
	*anypointer = "\nhello anypointer!\n";
	string ptrstr = *anypointer; //char* must not be == NULL or else undefined
	TypedSmartPointer<int> intpointer(new int());
	*intpointer = 256;
	int number = *intpointer;
	string numstr = to_string(number);
	string output = ptrstr + numstr + "\n";
	cout << output;
}

