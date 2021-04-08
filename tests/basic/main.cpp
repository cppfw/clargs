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
			utki::assert(std::string(e.what()) == "argument with short key 'a' already exists", [&](auto&o){o << e.what();}, SL);
		}
		utki::assert(exception_caught, SL);
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
			utki::assert(std::string(e.what()) == "argument with long key 'abrakadabra' already exists", [&](auto&o){o << e.what();}, SL);
		}
		utki::assert(exception_caught, SL);
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
			utki::assert(std::string(e.what()) == "argument with short key 'a' already exists", [&](auto&o){o << e.what();}, SL);
		}
		utki::assert(exception_caught, SL);
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
			utki::assert(std::string(e.what()) == "argument with long key 'abrakadabra' already exists", [&](auto&o){o << e.what();}, SL);
		}
		utki::assert(exception_caught, SL);
	}

	// test adding long key of one letter
	{
		clargs::parser p;

		p.add('a', "b", "description", [](){});

		try{
			p.add('b', "c", "description 2", [](std::string&&){});
		}catch(std::logic_error& e){
			utki::assert(false, SL);
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

		utki::assert(a == 3, [&](auto&o){o << "a = " << a;}, SL);
		utki::assert(res.size() == 3, [&](auto&o){o << "res.size() = " << res.size();}, SL);
		utki::assert(res[0] == "--aaa", [&](auto&o){o << "res[0] = " << res[0];}, SL);
		utki::assert(res[1] == "-a", [&](auto&o){o << "res[1] = " << res[1];}, SL);
		utki::assert(res[2] == "--aaa", [&](auto&o){o << "res[2] = " << res[2];}, SL);
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

		p.add([&res, &p](std::string&& str){
			res.push_back(std::move(str));
			if(res.size() == 3){
				p.enable_key_parsing(true);
			}
		});

		p.parse(utki::make_span(args));

		utki::assert(a == 3, [&](auto&o){o << "a = " << a;}, SL);
		utki::assert(res.size() == 3, [&](auto&o){o << "res.size() = " << res.size();}, SL);
		utki::assert(res[0] == "-a", [&](auto&o){o << "res[0] = " << res[0];}, SL);
		utki::assert(res[1] == "--aaa", [&](auto&o){o << "res[1] = " << res[1];}, SL);
		utki::assert(res[2] == "-a", [&](auto&o){o << "res[2] = " << res[2];}, SL);
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

		utki::assert(a == 2, [&](auto&o){o << "a = " << a;}, SL);
		utki::assert(res.size() == 4, [&](auto&o){o << "res.size() = " << res.size();}, SL);
		utki::assert(res[0] == "-a", [&](auto&o){o << "res[0] = " << res[0];}, SL);
		utki::assert(res[1] == "--aaa", [&](auto&o){o << "res[1] = " << res[1];}, SL);
		utki::assert(res[2] == "-a", [&](auto&o){o << "res[2] = " << res[2];}, SL);
		utki::assert(res[3] == "--aaa", [&](auto&o){o << "res[3] = " << res[3];}, SL);
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

		utki::assert(a == 6, [&](auto&o){o << "a = " << a;}, SL);
		utki::assert(res.size() == 0, [&](auto&o){o << "res.size() = " << res.size();}, SL);
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
			utki::assert(false, SL);
		}catch(std::invalid_argument& e){
			exception_caught = true;
			utki::assert(std::string(e.what()) == "unknown argument: --=", [&](auto&o){o << "e.what() = " << e.what();}, SL);
		}

		utki::assert(exception_caught, SL);
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
			utki::assert(std::string(e.what()) == "key argument '' is a boolean argument and cannot have value", [&](auto&o){o << "e.what() = " << e.what();}, SL);
		}

		utki::assert(!boolean_equals_handled, SL);
		utki::assert(exception_caught, SL);
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
			utki::assert(false, [&](auto&o){o << "e.what() = " << e.what();}, SL);
		}
		utki::assert(res.size() == 2, [&](auto&o){o << "res.size() = " << res.size();}, SL);
		utki::assert(res[0] == "", [&](auto&o){o << "res[0] = " << res[0];}, SL);
		utki::assert(res[1] == "blah blah", [&](auto&o){o << "res[1] = " << res[1];}, SL);
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

		utki::assert(res.size() == 2, [&](auto&o){o << "res.size() = " << res.size();}, SL);
		utki::assert(res[0] == "blah", [&](auto&o){o << "res[0] = " << res[0];}, SL);
		utki::assert(res[1] == "trololo", [&](auto&o){o << "res[1] = " << res[1];}, SL);
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

		utki::assert(res.size() == 3, [&](auto&o){o << "res.size() = " << res.size();}, SL);
		utki::assert(res[0] == "a", [&](auto&o){o << "res[0] = " << res[0];}, SL);
		utki::assert(res[1] == "b", [&](auto&o){o << "res[1] = " << res[1];}, SL);
		utki::assert(res[2] == "c", [&](auto&o){o << "res[2] = " << res[2];}, SL);
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

		utki::assert(res.size() == 3, [&](auto&o){o << "res.size() = " << res.size();}, SL);
		utki::assert(res[0] == "a", [&](auto&o){o << "res[0] = " << res[0];}, SL);
		utki::assert(res[1] == "b", [&](auto&o){o << "res[1] = " << res[1];}, SL);
		utki::assert(res[2] == "c = dkg", [&](auto&o){o << "res[2] = " << res[2];}, SL);
	}

	// test parsing of subcommands
	{
		clargs::parser p;

		std::vector<std::string> res;
		std::vector<std::string> subres;

		std::vector<const char*> args = {
			"program_executable",
			"-abcdkg",
			"subcommand",
			"-f f_val",
			"--hhh=h_val"
		};

		p.add('a', "aaa", "description", [&res](){res.push_back("a");});
		p.add('b', "description", [&res](){res.push_back("b");});
		p.add('c', "ccc", "description", [&res](std::string&& str){res.push_back(std::string("c = ") + std::move(str));});
		p.add('d', "ddd", "description", [&res](){res.push_back("d");});

		p.add([&res = subres](utki::span<const char* const> args){
			utki::assert(!args.empty(), SL);
			res.push_back(args.front());
			clargs::parser sp;

			sp.add('f', "fff", "description of fff", [&res](std::string&& v){res.push_back(std::string("f = ") + std::move(v));});
			sp.add('g', "ggg", "description of ggg", [&res](std::string&& v){res.push_back(std::string("g = ") + std::move(v));});
			sp.add('h', "hhh", "description of hhh", [&res](std::string&& v){res.push_back(std::string("h = ") + std::move(v));});

			sp.parse(args);
		});

		p.parse(utki::make_span(args));

		std::vector<std::string> expected = {
			"a",
			"b",
			"c = dkg"
		};

		utki::assert(res == expected, SL);

		std::vector<std::string> subexpected = {
			"subcommand",
			"f =  f_val",
			"h = h_val"
		};

		utki::assert(subres == subexpected, [&](auto&o){o << "subres.size() = " << subres.size() << ", subexpected.size() = " << subexpected.size();}, SL);
	}

	// test parsing of subcommands when key parsing is disabled
	{
		clargs::parser p;

		std::vector<std::string> res;
		std::vector<std::string> subres;

		std::vector<const char*> args = {
			"program_executable",
			"-abcdkg",
			"subcommand",
			"-f f_val",
			"--hhh=h_val"
		};

		p.add('a', "aaa", "description", [&res](){res.push_back("a");});
		p.add('b', "description", [&res](){res.push_back("b");});
		p.add('c', "ccc", "description", [&res](std::string&& str){res.push_back(std::string("c = ") + std::move(str));});
		p.add('d', "ddd", "description", [&res](){res.push_back("d");});

		p.add([&res](std::string&& v){res.push_back(std::move(v));});

		p.add([&res = subres](utki::span<const char* const> args){
			utki::assert(!args.empty(), SL);
			res.push_back(args.front());
			clargs::parser sp;

			sp.add('f', "fff", "description of fff", [&res](std::string&& v){res.push_back(std::string("f = ") + std::move(v));});
			sp.add('g', "ggg", "description of ggg", [&res](std::string&& v){res.push_back(std::string("g = ") + std::move(v));});
			sp.add('h', "hhh", "description of hhh", [&res](std::string&& v){res.push_back(std::string("h = ") + std::move(v));});

			sp.parse(args);
		});

		p.enable_key_parsing(false);

		p.parse(utki::make_span(args));

		std::vector<std::string> expected = {
			"-abcdkg",
			"subcommand",
			"-f f_val",
			"--hhh=h_val"
		};

		utki::assert(res == expected, SL);

		utki::assert(subres.empty(), [&](auto&o){o << "subres.size() = " << subres.size();}, SL);
	}

	return 0;
}
