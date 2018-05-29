#include "Args.hpp"

using namespace clargs;

void Args::add(
		char shortKey,
		const std::string& longKey,
		const std::string& description,
		std::function<void(std::string&&)>&& valueHandler
	)
{

}
