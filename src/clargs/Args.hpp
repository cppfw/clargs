#pragma once

#include <vector>
#include <functional>
#include <unordered_map>

#include <utki/Exc.hpp>

namespace clargs{

/**
 * @brief Unknown argument passed to command line.
 */
class UnknownArgumentExc : public utki::Exc{
public:
	UnknownArgumentExc(const std::string& message) :
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
class Args{
public:
	/**
	 * @brief Register command line argument.
	 * Registers command line agrument which has short one-letter name,
	 * long dash-separated name, description, and an argument value handling function.
	 * @param shortKey - one letter argument name.
	 * @param longKey - long, dash separated argument name.
	 * @param description - argument description.
	 * @param valueHandler - callback function which is called to handle value of the argument.
	 */
	void add(
			char shortKey,
			std::string&& longKey,
			std::string&& description,
			std::function<void(std::string&& value)>&& valueHandler
		);
	
	/**
	 * @brief Register command line argument.
	 * Registers command line agrument which has short one-letter name,
	 * description, and an argument value handling function.
	 * @param shortKey - one letter argument name.
	 * @param description - argument description.
	 * @param valueHandler - callback function which is called to handle value of the argument.
	 */
	void add(
			char shortKey,
			std::string&& description,
			std::function<void(std::string&& value)>&& valueHandler
		)
	{
		this->add(shortKey, std::string(), std::move(description), std::move(valueHandler));
	}
	
	/**
	 * @brief Register command line argument.
	 * Registers command line agrument which has
	 * long dash-separated name, description, and an argument value handling function.
	 * @param longKey - long, dash separated argument name.
	 * @param description - argument description.
	 * @param valueHandler - callback function which is called to handle value of the argument.
	 */
	void add(
			std::string&& longKey,
			std::string&& description,
			std::function<void(std::string&& value)>&& valueHandler
		)
	{
		this->add('\0', std::move(longKey), std::move(description), std::move(valueHandler));
	}
	
	/**
	 * @brief Register command line argument.
	 * Registers command line agrument which has short one-letter name,
	 * long dash-separated name, description, and an argument presence handling function.
	 * This argument does not have a value.
	 * @param shortKey - one letter argument name.
	 * @param longKey - long, dash separated argument name.
	 * @param description - argument description.
	 * @param valueHandler - callback function which is called to handle the argument presence in the command line.
	 */
	void add(
			char shortKey,
			std::string&& longKey,
			std::string&& description,
			std::function<void()>&& valueHandler
		);
	
	/**
	 * @brief Register command line argument.
	 * Registers command line agrument which has short one-letter name,
	 * description, and an argument presence handling function.
	 * This argument does not have a value.
	 * @param shortKey - one letter argument name.
	 * @param description - argument description.
	 * @param valueHandler - callback function which is called to handle the argument presence in the command line.
	 */
	void add(
			char shortKey,
			std::string&& description,
			std::function<void()>&& valueHandler
		)
	{
		this->add(shortKey, std::string(), std::move(description), std::move(valueHandler));
	}
	
	/**
	 * @brief Register command line argument.
	 * Registers command line agrument which has
	 * long dash-separated name, description, and an argument presence handling function.
	 * This argument does not have a value.
	 * @param longKey - long, dash separated argument name.
	 * @param description - argument description.
	 * @param valueHandler - callback function which is called to handle the argument presence in the command line.
	 */
	void add(
			std::string&& longKey,
			std::string&& description,
			std::function<void()>&& valueHandler
		)
	{
		this->add('\0', std::move(longKey), std::move(description), std::move(valueHandler));
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
	
	std::string addShortToLongMapping(char shortKey, std::string&& longKey);
	
	template <bool b> void addDescription(char shortKey, const std::string& longKey, std::string&& description);
	
	template <bool b> void addArgument(
			char shortKey,
			std::string&& longKey,
			std::string&& description,
			std::function<void(std::string&& value)>&& valueHandler
		);
	
	void parseLongKeyArgument(const std::string& arg);
};

}
