#include "../../src/clargs/args_t.hpp"

#include <iostream>

using namespace std;

int main(int argc, char** argv){
	clargs::args_t args;

	bool a = false;

	std::string removeSomething;

	std::string keyWithoutShortOne;
	std::string keyWithoutLongOne;

	args.add('a', "add-something", "adds something to somewhere", [&a](){a = true;});
	args.add('r', "remove-something", "removes something from somewhere", [&removeSomething](std::string&& s){removeSomething = std::move(s);});
	args.add("", "", [](){});
	args.add("key-without-short-one", "a key which has no short version, only the long one", [&keyWithoutShortOne](std::string&& s){keyWithoutShortOne = std::move(s);});
	args.add('b', "a key which has only a short version and does not have a long version", [&keyWithoutLongOne](std::string&& s){keyWithoutLongOne = std::move(s);});

	auto extras = args.parse(argc, argv);

	std::cout << args.description();

	std::cout << "a = " << (a ? "true" : "false") << std::endl;
	std::cout << "removeSomething = " << removeSomething << std::endl;
	std::cout << "keyWithoutShortOne = " << keyWithoutShortOne << std::endl;
	std::cout << "keyWithoutLongOne = " << keyWithoutLongOne << std::endl;

	for(auto& s : extras){
		std::cout << s << std::endl;
	}

	return 0;
}
