#include <tst/set.hpp>
#include <tst/check.hpp>

#include <clargs/parser.hpp>

using namespace std::string_literals;

namespace{
tst::set set("basic", [](tst::suite& suite){
    suite.add("add_same_key_twice__only_short_key", []{
        clargs::parser p;

		p.add('a', "description", [](){});

		bool exception_caught = false;

		try{
			p.add('a', "description 2", [](std::string_view){});
		}catch(std::logic_error& e){
			exception_caught = true;
            tst::check_eq(std::string(e.what()), "argument with short key 'a' already exists"s, SL) << e.what();
		}
		tst::check(exception_caught, SL);
    });

    suite.add("add_same_key_twice__only_long_key", []{
        clargs::parser p;

		p.add("abrakadabra", "description", [](){});

		bool exception_caught = false;

		try{
			p.add("abrakadabra", "description 2", [](std::string_view){});
		}catch(std::logic_error& e){
			exception_caught = true;
			tst::check_eq(std::string(e.what()), "argument with long key 'abrakadabra' already exists"s, SL) << e.what();
		}
		tst::check(exception_caught, SL);
    });

    suite.add("add_same_key_twice__same_short_key_and_different_long_key", []{
        clargs::parser p;

		p.add('a', "abrakadabra", "description", [](){});

		bool exception_caught = false;

		try{
			p.add('a', "simsalabim", "description 2", [](std::string_view){});
		}catch(std::logic_error& e){
			exception_caught = true;
            tst::check_eq(std::string(e.what()), "argument with short key 'a' already exists"s, SL) << e.what();
		}
		tst::check(exception_caught, SL);
    });

    suite.add("add_same_key_twice__different_short_key_and_same_long_key", []{
        clargs::parser p;

		p.add('a', "abrakadabra", "description", [](){});

		bool exception_caught = false;

		try{
			p.add('b', "abrakadabra", "description 2", [](std::string_view){});
		}catch(std::logic_error& e){
			exception_caught = true;
			tst::check_eq(std::string(e.what()), "argument with long key 'abrakadabra' already exists"s, SL) << e.what();
		}
		tst::check(exception_caught, SL);
    });

    suite.add("add_long_key_of_one_letter", []{
        clargs::parser p;

		p.add('a', "b", "description", [](){});

		try{
			p.add('b', "c", "description 2", [](std::string_view){});
		}catch(std::logic_error& e){
			tst::check(false, SL);
		}
    });

    suite.add("disabling_of_key_arguemnts_parsing", []{
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

		tst::check_eq(a, unsigned(3), SL) << "a = " << a;
		tst::check_eq(res.size(), size_t(3), SL) << " res.size() = " << res.size();
		tst::check_eq(res[0], "--aaa"s, SL) << "res[0] = " << res[0];
		tst::check_eq(res[1], "-a"s, SL) << "res[1] = " << res[1];
		tst::check_eq(res[2], "--aaa"s, SL) << "res[2] = " << res[2];
    });

    suite.add("enabling_of_key_arguemnts_parsing", []{
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

		p.add([&res, &p](std::string_view str){
			res.emplace_back(str);
			if(res.size() == 3){
				p.enable_key_parsing(true);
			}
		});

		p.parse(utki::make_span(args));

		tst::check_eq(a, unsigned(3), SL) << "a = " << a;
		tst::check_eq(res.size(), size_t(3), SL) << "res.size() = " << res.size();
		tst::check_eq(res[0], "-a"s, SL) << "res[0] = " << res[0];
		tst::check_eq(res[1], "--aaa"s, SL) << "res[1] = " << res[1];
		tst::check_eq(res[2], "-a"s, SL) << "res[2] = " << res[2];
    });

    suite.add("minus_minus_argument_disables_key_arguments_parsing", []{
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

		tst::check_eq(a, unsigned(2), SL) << "a = " << a;
		tst::check_eq(res.size(), size_t(4), SL) << "res.size() = " << res.size();
		tst::check_eq(res[0], "-a"s, SL) << "res[0] = " << res[0];
		tst::check_eq(res[1], "--aaa"s, SL) << "res[1] = " << res[1];
		tst::check_eq(res[2], "-a"s, SL) << "res[2] = " << res[2];
		tst::check_eq(res[3], "--aaa"s, SL) << "res[3] = " << res[3];
    });

	suite.add("override_minus_minus_argument_handling", []{
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

		tst::check_eq(a, unsigned(6), SL) << "a = " << a;
		tst::check_eq(res.size(), size_t(0), SL) << "res.size() = " << res.size();
	});

	suite.add("parsing_of_unadded_minus_minus_equals_argument", []{
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
			tst::check(false, SL);
		}catch(std::invalid_argument& e){
			exception_caught = true;
			tst::check_eq(std::string(e.what()), "unknown argument: --="s, SL) << "e.what() = " << e.what();
		}

		tst::check(exception_caught, SL);
	});

	suite.add("parsing_of_added_minus_minus_equals_boolean_argument", []{
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
			tst::check_eq(
					std::string(e.what()),
					"key argument '' is a boolean argument and cannot have value"s,
				SL) << "e.what() = " << e.what();
		}

		tst::check(!boolean_equals_handled, SL);
		tst::check(exception_caught, SL);
	});

	suite.add("parsing_of_added_minus_minus_equals_value_argument", []{
		clargs::parser p;

		unsigned a = 0;

		std::vector<std::string> res;

		p.add('a', "aaa", "description", [&a](){++a;});
		p.add("", "description", [&res](std::string_view str){res.emplace_back(str);});

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
			tst::check(false, SL) << "e.what() = " << e.what();
		}
		tst::check_eq(res.size(), size_t(2), SL) << "res.size() = " << res.size();
		tst::check_eq(res[0], ""s, SL) << "res[0] = " << res[0];
		tst::check_eq(res[1], "blah blah"s, SL) << "res[1] = " << res[1];
	});

	suite.add("parsing_of_concatenated_short_argument_and_its_value", []{
		clargs::parser p;

		std::vector<std::string> res;

		p.add('a', "aaa", "description", [&res](std::string_view str){res.emplace_back(str);});

		std::vector<const char*> args = {{
			"program_executable",
			"-ablah",
			"-atrololo"
		}};

		p.parse(utki::make_span(args));

		tst::check_eq(res.size(), size_t(2), SL) << "res.size() = " << res.size();
		tst::check_eq(res[0], "blah"s, SL) << "res[0] = " << res[0];
		tst::check_eq(res[1], "trololo"s, SL) << "res[1] = " << res[1];
	});

	suite.add("parsing_of_concatenated_boolean_arguments", []{
		clargs::parser p;

		std::vector<std::string> res;

		p.add('a', "aaa", "description", [&res](){res.emplace_back("a");});
		p.add('b', "description", [&res](){res.emplace_back("b");});
		p.add('c', "ccc", "description", [&res](){res.emplace_back("c");});

		std::vector<const char*> args = {{
			"program_executable",
			"-abc"
		}};

		p.parse(utki::make_span(args));

		tst::check_eq(res.size(), size_t(3), SL) << "res.size() = " << res.size();
		tst::check_eq(res[0], "a"s, SL) << "res[0] = " << res[0];
		tst::check_eq(res[1], "b"s, SL) << "res[1] = " << res[1];
		tst::check_eq(res[2], "c"s, SL) << "res[2] = " << res[2];
	});

	suite.add("parsing_of_concatenated_boolean_and_value_arguments", []{
		clargs::parser p;

		std::vector<std::string> res;

		p.add('a', "aaa", "description", [&res](){res.emplace_back("a");});
		p.add('b', "description", [&res](){res.emplace_back("b");});
		p.add('c', "ccc", "description", [&res](std::string_view str){res.push_back("c = "s.append(str));});
		p.add('d', "ddd", "description", [&res](){res.emplace_back("d");});

		std::vector<const char*> args = {{
			"program_executable",
			"-abcdkg"
		}};

		p.parse(utki::make_span(args));

		tst::check_eq(res.size(), size_t(3), SL) << "res.size() = " << res.size();
		tst::check_eq(res[0], "a"s, SL) << "res[0] = " << res[0];
		tst::check_eq(res[1], "b"s, SL) << "res[1] = " << res[1];
		tst::check_eq(res[2], "c = dkg"s, SL) << "res[2] = " << res[2];
	});

	suite.add("parsing_of_subcommands", []{
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

		p.add('a', "aaa", "description", [&res](){res.emplace_back("a");});
		p.add('b', "description", [&res](){res.emplace_back("b");});
		p.add('c', "ccc", "description", [&res](std::string_view str){res.push_back("c = "s.append(str));});
		p.add('d', "ddd", "description", [&res](){res.emplace_back("d");});

		p.add([&res = subres](utki::span<const char* const> args){
			tst::check(!args.empty(), SL);
			res.emplace_back(args.front());
			clargs::parser sp;

			sp.add('f', "fff", "description of fff", [&res](std::string_view v){res.push_back("f = "s.append(v));});
			sp.add('g', "ggg", "description of ggg", [&res](std::string_view v){res.push_back("g = "s.append(v));});
			sp.add('h', "hhh", "description of hhh", [&res](std::string_view v){res.push_back("h = "s.append(v));});

			sp.parse(args);
		});

		p.parse(utki::make_span(args));

		std::vector<std::string> expected = {
			"a",
			"b",
			"c = dkg"
		};

		tst::check(res == expected, SL);

		std::vector<std::string> subexpected = {
			"subcommand",
			"f =  f_val",
			"h = h_val"
		};

		tst::check(subres == subexpected, SL) << "subres.size() = " << subres.size() << ", subexpected.size() = " << subexpected.size();
	});

	suite.add("parsing_of_subcommands_when_key_parsing_is_disabled", []{
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

		p.add('a', "aaa", "description", [&res](){res.emplace_back("a");});
		p.add('b', "description", [&res](){res.emplace_back("b");});
		p.add('c', "ccc", "description", [&res](std::string_view str){res.push_back("c = "s.append(str));});
		p.add('d', "ddd", "description", [&res](){res.emplace_back("d");});

		p.add([&res](std::string_view v){res.emplace_back(v);});

		p.add([&res = subres](utki::span<const char* const> args){
			tst::check(!args.empty(), SL);
			res.emplace_back(args.front());
			clargs::parser sp;

			sp.add('f', "fff", "description of fff", [&res](std::string_view v){res.push_back("f = "s.append(v));});
			sp.add('g', "ggg", "description of ggg", [&res](std::string_view v){res.push_back("g = "s.append(v));});
			sp.add('h', "hhh", "description of hhh", [&res](std::string_view v){res.push_back("h = "s.append(v));});

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

		tst::check(res == expected, SL);

		tst::check(subres.empty(), SL) << "subres.size() = " << subres.size();
	});
});
}
