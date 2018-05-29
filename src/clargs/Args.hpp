#pragma once

#include <vector>
#include <functional>
#include <unordered_map>

namespace clargs{

class Args{
public:
	Args();
	
	void add(
			char shortKey,
			const std::string& longKey,
			const std::string& description,
			std::function<void(std::string&& value)>&& valueHandler
		);
	
	void add(
			char shortKey,
			const std::string& longKey,
			const std::string& description,
			std::function<void()>&& valueHandler
		);
	
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
	std::unordered_map<std::string, std::function<void(std::string&&)>> valueArgs;
	std::unordered_map<std::string, std::function<void()>> boolArgs;
	
	std::vector<std::string> argDescriptions;
};

}
