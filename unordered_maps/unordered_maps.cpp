#include <iostream>
#include <unordered_map>
using std::string, std::unordered_map, std::cout, std::make_pair, std::pair;
int main()
{
	unordered_map<string, int> umap;
	umap["Key1"] = 2048;//add keys one at a time this way
	umap["Key2"] = 1024;
	umap["Key3"] = 512;
	umap["Key4"] = 256;
	umap["Key5"] = 2058;
        umap["Key6"] = 1044;
        umap["Key7"] = 513;
        umap["Key8"] = 255;
	umap["Key9"] = 2088;
        umap["Key10"] = 1004;
        umap["Key11"] = 516;
        umap["Key12"] = 250;
        umap["Key13"] = 513462;
        umap["Key14"] = 253466;//we can enter these in order if we like but they will output out of order
	for (auto i : umap)
	{
		cout << i.first << " " << i.second << "\n";
	}
	umap.insert(make_pair("Value15", 710));//insert will add a key in a similar way to 'umap[Key] = Value;'
	string key = "Key9";
	if (umap.find(key) == umap.end())//if umap.find reaches umap.end
	{
		cout << key + " does not exist inside the unordered map!\n";
	}
	else
	{
		cout << key + " found inside the unordered map!\n";
		cout << key + "'s value:	" << umap[key] << "\n";//getting the value of the key
	}
	auto print_key_value = [](auto& key, auto& value)//helper to print the value of any key in the map
	{
		cout << "Key:	" << key << "\nValue:	" << value << "\n";
	};
	cout << "\nexplicit iteration:\n\n";
	for (const pair<string, int>& i : umap)//iterating using explicit types of the mapped contents
	{	
		print_key_value(i.first, i.second);
	}
	cout << "\ngeneral iteration:\n\n";
	for (const auto& [key, value] : umap)//non explicit C++17 structured binding
	{
		print_key_value(key, value);
	}
	umap.clear();//empty the container
	return 0;
}
