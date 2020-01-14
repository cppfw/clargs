#pragma once

#include <vector>
#include <functional>
#include <unordered_map>

#include <utki/span.hpp>

namespace clargs{

/**
 * @brief Parser of command line arguments.
 * This class represents a parser of command line arguments.
 * It holds information about all known command line arguments with corresponding
 * handler functions. When parsing command line aruments it calls user supplied callback
 * functions for each encountered known argument from command line.
 */
class parser{
public:
	/**
	 * @brief Register command line argument.
	 * Registers command line agrument which has short one-letter name,
	 * long dash-separated name, description, and an argument value handling function.
	 * @param short_key - one letter argument name.
	 * @param long_key - long, dash separated argument name.
	 * @param description - argument description.
	 * @param value_handler - callback function which is called to handle value of the argument.
	 */
	void add(
			char short_key,
			std::string&& long_key,
			std::string&& description,
			std::function<void(std::string&&)>&& value_handler
		)
	{
		this->add_argument(
				short_key,
				std::move(long_key),
				std::move(description),
				std::move(value_handler),
				nullptr
			);
	}

	/**
	 * @brief Register command line argument.
	 * Registers command line agrument which has short one-letter name,
	 * description, and an argument value handling function.
	 * @param short_key - one letter argument name.
	 * @param description - argument description.
	 * @param value_handler - callback function which is called to handle value of the argument.
	 */
	void add(
			char short_key,
			std::string&& description,
			std::function<void(std::string&&)>&& value_handler
		)
	{
		this->add(short_key, std::string(), std::move(description), std::move(value_handler));
	}

	/**
	 * @brief Register command line argument.
	 * Registers command line agrument which has
	 * long dash-separated name, description, and an argument value handling function.
	 * @param long_key - long, dash separated argument name.
	 * @param description - argument description.
	 * @param value_handler - callback function which is called to handle value of the argument.
	 * @param default_value_handler - callback function which is called when the argument has not value given.
	 */
	void add(
			std::string&& long_key,
			std::string&& description,
			std::function<void(std::string&&)>&& value_handler,
			std::function<void()>&& default_value_handler = nullptr
		)
	{
		this->add_argument(
				'\0',
				std::move(long_key),
				std::move(description),
				std::move(value_handler),
				std::move(default_value_handler)
			);
	}

	/**
	 * @brief Register command line argument.
	 * Registers command line agrument which has short one-letter name,
	 * long dash-separated name, description, and an argument presence handling function.
	 * This argument does not have a value.
	 * @param short_key - one letter argument name.
	 * @param long_key - long, dash separated argument name.
	 * @param description - argument description.
	 * @param boolean_handler - callback function which is called to handle the argument presence in the command line.
	 */
	void add(
			char short_key,
			std::string&& long_key,
			std::string&& description,
			std::function<void()>&& boolean_handler
		)
	{
		this->add_argument(
				short_key,
				std::move(long_key),
				std::move(description),
				nullptr,
				std::move(boolean_handler)
			);
	}

	/**
	 * @brief Register command line argument.
	 * Registers command line agrument which has short one-letter name,
	 * description, and an argument presence handling function.
	 * This argument does not have a value.
	 * @param short_key - one letter argument name.
	 * @param description - argument description.
	 * @param boolean_handler - callback function which is called to handle the argument presence in the command line.
	 */
	void add(
			char short_key,
			std::string&& description,
			std::function<void()>&& boolean_handler
		)
	{
		this->add(short_key, std::string(), std::move(description), std::move(boolean_handler));
	}

	/**
	 * @brief Register command line argument.
	 * Registers command line agrument which has
	 * long dash-separated name, description, and an argument presence handling function.
	 * This argument does not have a value.
	 * @param long_key - long, dash separated argument name.
	 * @param description - argument description.
	 * @param value_handler - callback function which is called to handle the argument presence in the command line.
	 */
	void add(
			std::string&& long_key,
			std::string&& description,
			std::function<void()>&& value_handler
		)
	{
		this->add('\0', std::move(long_key), std::move(description), std::move(value_handler));
	}

	/**
	 * @brief Enable or disable key arguments parsing.
	 * By default key arguments parsing is enabled.
	 * If key arguments parsing is disabled, then all the arguments will be treated as non-key arguments.
	 * By default, after encountering '--' argument the key arguments parsing is disabled, user can override this
	 * behaviour by overriding handling of long-name-only argument with empty long name.
	 * It is ok to call this function from within the arguments handling callback functions.
	 * @param enable - if true, key arguments parsing will be enabled, otherwise - disabled.
	 */
	void enable_key_parsing(bool enable)noexcept{
		this->is_key_parsing_enabled = enable;
	}

	/**
	 * @brief Parse command line arguments.
	 * Parses the command line arguments as they passed in to main() function.
	 * Zeroth argument is the filename of the executable.
	 * @param args - array of command line arguments.
	 * @param non_key_handler - handler callback for non-key arguments. Can be nullptr.
	 */
	void parse(const utki::span<const char*> args, std::function<void(std::string&&)> non_key_handler);

	/**
	 * @brief Parse command line arguments.
	 * Parses the command line arguments as they passed in to main() function.
	 * Zeroth argument is the filename of the executable.
	 * @param args - array of command line arguments.
	 * @return array of non-key arguments.
	 */
	std::vector<std::string> parse(const utki::span<const char*> args);

	/**
	 * @brief Parse command line arguments.
	 * Parses the command line arguments as they passed in to main() function.
	 * Zeroth argument is the filename of the executable.
	 * @param argc - number of arguments.
	 * @param argv - array of arguments.
	 * @return array of non-key arguments.
	 */
	std::vector<std::string> parse(int argc, const char* const* argv){
		return this->parse(utki::make_span(argv, argc));
	}

	/**
	 * @brief Get description of the arguments.
	 * There will be 2 characters gap between key names and key description.
	 * @param keys_width - width in characters of the key names area.
	 * @param width - width in characters of key description area.
	 * @return Formatted description of all the registered arguments.
	 */
	std::string description(unsigned keys_width = 38, unsigned width = 40);

private:
	bool is_key_parsing_enabled = true;

	struct argument_callbacks{
		std::function<void(std::string&&)> value_handler;
		std::function<void()> boolean_handler;
	};

	std::unordered_map<std::string, argument_callbacks> arguments;

	std::unordered_map<char, std::string> short_to_long_map;

	struct key_description{
		std::string key_names;
		std::string description;
	};

	std::vector<key_description> key_descriptions;

	std::string add_short_to_long_mapping(char short_key, std::string&& long_key);

	void push_back_description(
			char short_key,
			const std::string& long_key,
			std::string&& description,
			bool is_boolean,
			bool is_value_optional
		);

	void add_argument(
			char short_key,
			std::string&& long_key,
			std::string&& description,
			std::function<void(std::string&&)>&& value_handler,
			std::function<void()>&& boolean_handler
		);

	void parse_long_key_argument(const std::string& arg);

	// returns pointer to last argument's value handler in case value is the next argument.
	// returns nullptr otherwise.
	std::function<void(std::string&&)>* parse_short_keys_batch(const std::string& arg);
};

}
