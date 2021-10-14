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
			p.add('a', "description 2", [](std::string&&){});
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
			p.add("abrakadabra", "description 2", [](std::string&&){});
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
			p.add('a', "simsalabim", "description 2", [](std::string&&){});
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
			p.add('b', "abrakadabra", "description 2", [](std::string&&){});
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
			p.add('b', "c", "description 2", [](std::string&&){});
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

		p.add([&res, &p](std::string&& str){
			res.push_back(std::move(str));
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
});
}
