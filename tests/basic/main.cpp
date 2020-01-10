#include "../../src/clargs/parser.hpp"

#include <utki/debug.hpp>
#include <utki/exception.hpp>


int main(int argc, char** argv){
	// test adding same key twice (only short key)
	{
		clargs::parser p;

		p.add('a', "description", [](){});

		bool exception_caught = false;

		try{
			p.add('a', "description 2", [](std::string&&){});
		}catch(utki::invalid_state& e){
			exception_caught = true;
			ASSERT_INFO_ALWAYS(std::string(e.what()) == "argument with short key 'a' already exists", e.what())
		}
		ASSERT_ALWAYS(exception_caught)	
	}
	
	// test adding same key twice (only long key)
	{
		clargs::parser p;

		p.add("abrakadabra", "description", [](){});

		bool exception_caught = false;

		try{
			p.add("abrakadabra", "description 2", [](std::string&&){});
		}catch(utki::invalid_state& e){
			exception_caught = true;
			ASSERT_INFO_ALWAYS(std::string(e.what()) == "argument with long key 'abrakadabra' already exists", e.what())
		}
		ASSERT_ALWAYS(exception_caught)	
	}

	// test adding same key twice (same short key and different long key)
	{
		clargs::parser p;

		p.add('a', "abrakadabra", "description", [](){});

		bool exception_caught = false;

		try{
			p.add('a', "simsalabim", "description 2", [](std::string&&){});
		}catch(utki::invalid_state& e){
			exception_caught = true;
			ASSERT_INFO_ALWAYS(std::string(e.what()) == "argument with short key 'a' already exists", e.what())
		}
		ASSERT_ALWAYS(exception_caught)	
	}

	// test adding same key twice (different short key and same long key)
	{
		clargs::parser p;

		p.add('a', "abrakadabra", "description", [](){});

		bool exception_caught = false;

		try{
			p.add('b', "abrakadabra", "description 2", [](std::string&&){});
		}catch(utki::invalid_state& e){
			exception_caught = true;
			ASSERT_INFO_ALWAYS(std::string(e.what()) == "argument with long key 'abrakadabra' already exists", e.what())
		}
		ASSERT_ALWAYS(exception_caught)	
	}

	// test adding long key of one letter
	{
		clargs::parser p;

		p.add('a', "b", "description", [](){});

		try{
			p.add('b', "c", "description 2", [](std::string&&){});
		}catch(utki::invalid_state& e){
			ASSERT_ALWAYS(false)
		}
	}

	return 0;
}
