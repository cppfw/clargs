#pragma once

#include <vector>
#include <functional>
#include <unordered_map>

namespace clargs{

class Args{
public:
	void add(
			char shortKey,
			std::string&& longKey,
			std::string&& description,
			std::function<void(std::string&& value)>&& valueHandler
		);
	
	void add(
			char shortKey,
			std::string&& description,
			std::function<void(std::string&& value)>&& valueHandler
		)
	{
		this->add(shortKey, std::string(), std::move(description), std::move(valueHandler));
	}
	
	void add(
			std::string&& longKey,
			std::string&& description,
			std::function<void(std::string&& value)>&& valueHandler
		)
	{
		this->add('\0', std::move(longKey), std::move(description), std::move(valueHandler));
	}
	
	void add(
			char shortKey,
			std::string&& longKey,
			std::string&& description,
			std::function<void()>&& valueHandler
		);
	
	void add(
			char shortKey,
			std::string&& description,
			std::function<void()>&& valueHandler
		)
	{
		this->add(shortKey, std::string(), std::move(description), std::move(valueHandler));
	}
	
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
	 * @return Formatted description of the arguments.
	 */
	std::string description();
	
private:
	std::string addShortToLongMapping(char shortKey, std::string&& longKey);
	
	template <bool b> void addDescription(char shortKey, const std::string& longKey, std::string&& description);
	
	std::unordered_map<std::string, std::function<void(std::string&&)>> valueArgs;
	std::unordered_map<std::string, std::function<void()>> boolArgs;
	
	std::unordered_map<char, std::string> shortToLongMap;
	
	std::vector<std::string> argDescriptions;
};

}
