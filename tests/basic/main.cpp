#include "../../src/clargs/parser.hpp"

#include <utki/debug.hpp>

int main(int argc, char** argv){
	// test adding same key twice (only short key)
	{
		clargs::parser p;

		p.add('a', "description", [](){});

		bool exception_caught = false;

		try{
			p.add('a', "description 2", [](std::string&&){});
		}catch(std::logic_error& e){
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
		}catch(std::logic_error& e){
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
		}catch(std::logic_error& e){
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
		}catch(std::logic_error& e){
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
		}catch(std::logic_error& e){
			ASSERT_ALWAYS(false)
		}
	}

	// test disabling of key arguemnts parsing
	{
		clargs::parser p;

		unsigned a = 0;
		
		p.add('a', "aaa", "description", [&a, &p](){
			++a;
			if(a == 3){
				p.enable_key_parsing(false);
			}
		});

		std::vector<const char*> args = {{
			"program_executable",
			"-a",
			"--aaa",
			"-a",
			"--aaa",
			"-a",
			"--aaa"
		}};

		auto res = p.parse(utki::make_span(args));

		ASSERT_INFO_ALWAYS(a == 3, "a = " << a)
		ASSERT_INFO_ALWAYS(res.size() == 3, "res.size() = " << res.size())
		ASSERT_INFO_ALWAYS(res[0] == "--aaa", "res[0] = " << res[0])
		ASSERT_INFO_ALWAYS(res[1] == "-a", "res[1] = " << res[1])
		ASSERT_INFO_ALWAYS(res[2] == "--aaa", "res[2] = " << res[2])
	}

	// test enabling of key arguemnts parsing
	{
		clargs::parser p;

		unsigned a = 0;
		
		p.add('a', "aaa", "description", [&a, &p](){
			++a;
			if(a == 2){
				p.enable_key_parsing(false);
			}
		});

		std::vector<const char*> args = {{
			"program_executable",
			"-a",
			"--aaa",
			"-a",
			"--aaa",
			"-a",
			"--aaa"
		}};

		std::vector<std::string> res;

		p.parse(utki::make_span(args), [&res, &p](std::string&& str){
			res.push_back(std::move(str));
			if(res.size() == 3){
				p.enable_key_parsing(true);
			}
		});

		ASSERT_INFO_ALWAYS(a == 3, "a = " << a)
		ASSERT_INFO_ALWAYS(res.size() == 3, "res.size() = " << res.size())
		ASSERT_INFO_ALWAYS(res[0] == "-a", "res[0] = " << res[0])
		ASSERT_INFO_ALWAYS(res[1] == "--aaa", "res[1] = " << res[1])
		ASSERT_INFO_ALWAYS(res[2] == "-a", "res[2] = " << res[2])
	}

	// test that -- argument disables key arguments parsing
	{
		clargs::parser p;

		unsigned a = 0;
		
		p.add('a', "aaa", "description", [&a](){
			++a;
		});

		std::vector<const char*> args = {{
			"program_executable",
			"-a",
			"--aaa",
			"--",
			"-a",
			"--aaa",
			"-a",
			"--aaa"
		}};

		auto res = p.parse(utki::make_span(args));

		ASSERT_INFO_ALWAYS(a == 2, "a = " << a)
		ASSERT_INFO_ALWAYS(res.size() == 4, "res.size() = " << res.size())
		ASSERT_INFO_ALWAYS(res[0] == "-a", "res[0] = " << res[0])
		ASSERT_INFO_ALWAYS(res[1] == "--aaa", "res[1] = " << res[1])
		ASSERT_INFO_ALWAYS(res[2] == "-a", "res[2] = " << res[2])
		ASSERT_INFO_ALWAYS(res[3] == "--aaa", "res[3] = " << res[3])
	}

	// test overriding the '--' argument handling
	{
		clargs::parser p;

		unsigned a = 0;
		
		p.add('a', "aaa", "description", [&a](){++a;});
		p.add("", "", [](){});

		std::vector<const char*> args = {{
			"program_executable",
			"-a",
			"--aaa",
			"--",
			"-a",
			"--aaa",
			"-a",
			"--aaa"
		}};

		auto res = p.parse(utki::make_span(args));

		ASSERT_INFO_ALWAYS(a == 6, "a = " << a)
		ASSERT_INFO_ALWAYS(res.size() == 0, "res.size() = " << res.size())
	}

	// test parsing of unadded '--=' argument
	{
		clargs::parser p;

		unsigned a = 0;
		
		p.add('a', "aaa", "description", [&a](){++a;});

		std::vector<const char*> args = {{
			"program_executable",
			"-a",
			"--aaa",
			"--=",
			"-a",
			"--aaa",
			"-a",
			"--aaa"
		}};

		bool exception_caught = false;
		try{
			p.parse(utki::make_span(args));
			ASSERT_ALWAYS(false)
		}catch(std::invalid_argument& e){
			exception_caught = true;
			ASSERT_INFO_ALWAYS(std::string(e.what()) == "unknown argument: --=", "e.what() = " << e.what())
		}

		ASSERT_ALWAYS(exception_caught)
	}

	// test parsing of added '--=' boolean argument
	{
		clargs::parser p;

		unsigned a = 0;
		
		bool boolean_equals_handled = false;

		p.add('a', "aaa", "description", [&a](){++a;});
		p.add("", "description", [&boolean_equals_handled](){boolean_equals_handled = true;});

		std::vector<const char*> args = {{
			"program_executable",
			"-a",
			"--aaa",
			"--=",
			"-a",
			"--aaa",
			"-a",
			"--aaa"
		}};

		bool exception_caught = false;
		try{
			p.parse(utki::make_span(args));
		}catch(std::invalid_argument& e){
			exception_caught = true;
			ASSERT_INFO_ALWAYS(std::string(e.what()) == "key argument '' is a boolean argument and cannot have value", "e.what() = " << e.what())
		}

		ASSERT_ALWAYS(!boolean_equals_handled)
		ASSERT_ALWAYS(exception_caught)
	}

	// test parsing of added '--=' value argument
	{
		clargs::parser p;

		unsigned a = 0;

		std::vector<std::string> res;

		p.add('a', "aaa", "description", [&a](){++a;});
		p.add("", "description", [&res](std::string&& str){res.push_back(std::move(str));});

		std::vector<const char*> args = {{
			"program_executable",
			"-a",
			"--aaa",
			"--=",
			"-a",
			"--=blah blah",
			"--aaa",
			"-a",
			"--aaa"
		}};

		try{
			p.parse(utki::make_span(args));
		}catch(std::invalid_argument& e){
			ASSERT_INFO_ALWAYS(false, "e.what() = " << e.what())
		}
		ASSERT_INFO_ALWAYS(res.size() == 2, "res.size() = " << res.size())
		ASSERT_INFO_ALWAYS(res[0] == "", "res[0] = " << res[0])
		ASSERT_INFO_ALWAYS(res[1] == "blah blah", "res[1] = " << res[1])
	}

	// test parsing of concatenated short argument and its value
	{
		clargs::parser p;

		std::vector<std::string> res;

		p.add('a', "aaa", "description", [&res](std::string&& str){res.push_back(std::move(str));});

		std::vector<const char*> args = {{
			"program_executable",
			"-ablah",
			"-atrololo"
		}};

		p.parse(utki::make_span(args));

		ASSERT_INFO_ALWAYS(res.size() == 2, "res.size() = " << res.size())
		ASSERT_INFO_ALWAYS(res[0] == "blah", "res[0] = " << res[0])
		ASSERT_INFO_ALWAYS(res[1] == "trololo", "res[1] = " << res[1])
	}

	// test parsing of concatenated boolean arguments
	{
		clargs::parser p;

		std::vector<std::string> res;

		p.add('a', "aaa", "description", [&res](){res.push_back("a");});
		p.add('b', "description", [&res](){res.push_back("b");});
		p.add('c', "ccc", "description", [&res](){res.push_back("c");});

		std::vector<const char*> args = {{
			"program_executable",
			"-abc"
		}};

		p.parse(utki::make_span(args));

		ASSERT_INFO_ALWAYS(res.size() == 3, "res.size() = " << res.size())
		ASSERT_INFO_ALWAYS(res[0] == "a", "res[0] = " << res[0])
		ASSERT_INFO_ALWAYS(res[1] == "b", "res[1] = " << res[1])
		ASSERT_INFO_ALWAYS(res[2] == "c", "res[2] = " << res[2])
	}

	// test parsing of concatenated boolean and value arguments
	{
		clargs::parser p;

		std::vector<std::string> res;

		p.add('a', "aaa", "description", [&res](){res.push_back("a");});
		p.add('b', "description", [&res](){res.push_back("b");});
		p.add('c', "ccc", "description", [&res](std::string&& str){res.push_back(std::string("c = ") + std::move(str));});
		p.add('d', "ddd", "description", [&res](){res.push_back("d");});

		std::vector<const char*> args = {{
			"program_executable",
			"-abcdkg"
		}};

		p.parse(utki::make_span(args));

		ASSERT_INFO_ALWAYS(res.size() == 3, "res.size() = " << res.size())
		ASSERT_INFO_ALWAYS(res[0] == "a", "res[0] = " << res[0])
		ASSERT_INFO_ALWAYS(res[1] == "b", "res[1] = " << res[1])
		ASSERT_INFO_ALWAYS(res[2] == "c = dkg", "res[2] = " << res[2])
	}

	return 0;
}
