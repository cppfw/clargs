#include "../../src/clargs/parser.hpp"

#include <iostream>


int main(int argc, char** argv){
	clargs::parser args;

	bool a = false;
	std::string remove_something;
	std::string key_without_short_one;
	std::string key_without_long_one;
	std::string key_empty_value = "ASSERT_should_be_empty!";
	std::string key_with_optional_value;
	std::string another_key_with_optional_value;
	std::string key_to_test_long_key_quoted_value;
	std::string key_to_test_short_key_quoted_value;
	std::string key_to_test_short_key_space_quoted_value;
	bool boolean_long_key_without_short_one = false;

	args.add('a', "add-something", "adds something to somewhere", [&a](){a = true;});
	args.add(
			'r',
			"remove-something",
			"removes something from somewhere",
			[&remove_something](std::string_view s){
				remove_something = s;
			}
		);
	args.add(
			"key-without-short-one",
			"a key which has no short version, only the long one",
			[&key_without_short_one](std::string_view s){
				key_without_short_one = s;
			}
		);
	args.add(
			'b',
			"a key which has only a short version and does not have a long version",
			[&key_without_long_one](std::string_view s){
				key_without_long_one = s;
			}
		);
	args.add(
			"key-empty-value",
			"a key to test passing in an empty value",
			[&key_empty_value](std::string_view v){
				key_empty_value = v;
			}
		);
	args.add(
			"key-with-optional-value",
			"a key which has optional value",
			[&key_with_optional_value](std::string_view v){key_with_optional_value = v;},
			[&key_with_optional_value](){key_with_optional_value = "key_with_optional_value_ASSERT_FALSE";}
		);
	args.add(
			"another-key-with-optional-value",
			"a key which has optional value, another one",
			[&another_key_with_optional_value](std::string_view v){another_key_with_optional_value = v;},
			[&another_key_with_optional_value](){another_key_with_optional_value = "another_key_with_optional_value_DEFAULT_VALUE";}
		);
	args.add(
			"key-to-test-quoted-value",
			"a key to test passing in quoted value as long key argumeent",
			[&key_to_test_long_key_quoted_value](std::string_view v){key_to_test_long_key_quoted_value = v;}
		);
	args.add(
			'q',
			"a key to test passing in quoted value as short key argument",
			[&key_to_test_short_key_quoted_value](std::string_view v){
				key_to_test_short_key_quoted_value = v;
			}
		);
	args.add(
			'd',
			"a key to test passing in quoted value as short key argument, with space",
			[&key_to_test_short_key_space_quoted_value](std::string_view v){
				key_to_test_short_key_space_quoted_value = v;
			}
		);
	args.add(
			"boolean-long-key-without-short-one",
			"a boolean long key without short key",
			[&boolean_long_key_without_short_one](){boolean_long_key_without_short_one = true;}
		);

	auto extras = args.parse(argc, argv);

	std::cout << args.description();

	std::cout << "a = " << (a ? "true" : "false") << std::endl;
	std::cout << "remove_something = " << remove_something << std::endl;
	std::cout << "key_without_short_one = " << key_without_short_one << std::endl;
	std::cout << "key_without_long_one = " << key_without_long_one << std::endl;
	std::cout << "key_empty_value = " << key_empty_value << std::endl;
	std::cout << "key_with_optional_value = " << key_with_optional_value << std::endl;
	std::cout << "another_key_with_optional_value = " << another_key_with_optional_value << std::endl;
	std::cout << "key_to_test_long_key_quoted_value = " << key_to_test_long_key_quoted_value << std::endl;
	std::cout << "key_to_test_short_key_quoted_value = " << key_to_test_short_key_quoted_value << std::endl;
	std::cout << "key_to_test_short_key_space_quoted_value = " << key_to_test_short_key_space_quoted_value << std::endl;
	std::cout << "boolean_long_key_without_short_one = " << (boolean_long_key_without_short_one ? "true" : "false") << std::endl;

	for(auto& s : extras){
		std::cout << s << std::endl;
	}

	return 0;
}
