#pragma once

#include <vector>
#include <functional>

namespace clargs{

class Args{
public:
	Args(const std::string& programOneLineDescription);
	
	Args(const Args&) = delete;
	Args& operator=(const Args&) = delete;
	
	
	void add(
			const std::string& shortKey,
			const std::string& longKey,
			const std::string& description,
			std::function<void(std::string&& value)>&& valueHandler
		);
	
	void add(
			const std::string& shortKey,
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
	
private:
	
};

}
