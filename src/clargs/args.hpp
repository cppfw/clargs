#pragma once

#include <vector>
#include <functional>
#include <unordered_map>

#include <utki/Exc.hpp>

namespace clargs{

/**
 * @brief Unknown argument passed to command line.
 */
class unknown_argument_exception : public utki::Exc{
public:
	unknown_argument_exception(const std::string& message) :
			utki::Exc(message)
	{}
};

/**
 * @brief Parser of command line arguments.
 * This class represents a parser of command line arguments.
 * It holds information about all known command line arguments with corresponding
 * handler functions. When parsing command line aruments it calls user supplied callback
 * functions for each encountered known argument from command line.
 */
class args{
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
			std::function<void(std::string&& value)>&& value_handler
		);

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
			std::function<void(std::string&& value)>&& value_handler
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
	 */
	void add(
			std::string&& long_key,
			std::string&& description,
			std::function<void(std::string&& value)>&& value_handler
		)
	{
		this->add('\0', std::move(long_key), std::move(description), std::move(value_handler));
	}

	/**
	 * @brief Register command line argument.
	 * Registers command line agrument which has short one-letter name,
	 * long dash-separated name, description, and an argument presence handling function.
	 * This argument does not have a value.
	 * @param short_key - one letter argument name.
	 * @param long_key - long, dash separated argument name.
	 * @param description - argument description.
	 * @param value_handler - callback function which is called to handle the argument presence in the command line.
	 */
	void add(
			char short_key,
			std::string&& long_key,
			std::string&& description,
			std::function<void()>&& value_handler
		);

	/**
	 * @brief Register command line argument.
	 * Registers command line agrument which has short one-letter name,
	 * description, and an argument presence handling function.
	 * This argument does not have a value.
	 * @param short_key - one letter argument name.
	 * @param description - argument description.
	 * @param value_handler - callback function which is called to handle the argument presence in the command line.
	 */
	void add(
			char short_key,
			std::string&& description,
			std::function<void()>&& value_handler
		)
	{
		this->add(short_key, std::string(), std::move(description), std::move(value_handler));
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
	 * @brief Parse command line arguments.
	 * Parses the command line arguments as they passed in to main() function.
	 * First argument is the filename of the executable.
	 * @param argc - number of arguments.
	 * @param argv - array of arguments.
	 * @return list of non-key arguments.
	 */
	std::vector<std::string> parse(int argc, char** argv);

	/**
	 * @brief Get description of the arguments.
	 * @return Formatted description of all the registered arguments.
	 */
	std::string description();

private:
	std::unordered_map<std::string, std::function<void(std::string&&)>> valueArgs;
	std::unordered_map<std::string, std::function<void(std::string&&)>> boolArgs;

	std::unordered_map<char, std::string> shortToLongMap;

	std::vector<std::string> argDescriptions;

	std::string add_short_to_long_mapping(char short_key, std::string&& long_key);

	template <bool b> void add_description(char short_key, const std::string& long_key, std::string&& description);

	template <bool b> void add_argument(
			char short_key,
			std::string&& long_key,
			std::string&& description,
			std::function<void(std::string&& value)>&& value_handler
		);

	void parse_long_key_argument(const std::string& arg);
};

}
