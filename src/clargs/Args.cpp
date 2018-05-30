#include <sstream>

#include <utki/util.hpp>

#include "Args.hpp"

using namespace clargs;


template <bool b> void Args::addDescription(char shortKey, const std::string& longKey, std::string&& description) {
	std::stringstream ss;
	ss << "  ";
	
	if(shortKey != 0){
		ss << '-' << shortKey;
		if(!b){
			ss << " VALUE";
		}
	}
	
	if(longKey.size() != 0){
		if(shortKey != 0){
			ss << ", ";
		}
		ss << "--" << longKey;
		if(!b){
			ss << "=VALUE";
		}
	}

	const unsigned descriptionNewlineThreshold_c = 38;
	
	unsigned keysLength = ss.tellp();
	if(keysLength > descriptionNewlineThreshold_c){
		ss << std::endl << std::string(descriptionNewlineThreshold_c, ' ');
	}else{
		ss << std::string(descriptionNewlineThreshold_c - keysLength, ' ');
	}
	
	ss << "  " << description << "." << std::endl;
	
	this->argDescriptions.emplace_back(ss.str());
}

std::string Args::addShortToLongMapping(char shortKey, std::string&& longKey) {
	std::string k;
	if(longKey.size() != 0){
		k = std::move(longKey);
		
		if(shortKey != 0){
			this->shortToLongMap.insert(std::make_pair(shortKey, k));
		}
	}else{
		k = shortKey;
	}
	return k;
}


void Args::add(
		char shortKey,
		std::string&& longKey,
		std::string&& description,
		std::function<void(std::string&&)>&& valueHandler
	)
{
	this->addDescription<false>(shortKey, longKey, std::move(description));
	utki::ScopeExit descriptionScopeExit([this](){this->argDescriptions.pop_back();});
	
	auto k = this->addShortToLongMapping(shortKey, std::move(longKey));
	utki::ScopeExit mappingScopeExit([this, shortKey](){
		this->shortToLongMap.erase(shortKey);
	});
	
	this->valueArgs.insert(std::make_pair(std::move(k), std::move(valueHandler)));
	
	mappingScopeExit.reset();
	descriptionScopeExit.reset();
}

void Args::add(
		char shortKey,
		std::string&& longKey,
		std::string&& description,
		std::function<void()>&& valueHandler
	)
{
	this->addDescription<true>(shortKey, longKey, std::move(description));
	utki::ScopeExit descriptionScopeExit([this](){this->argDescriptions.pop_back();});
	
	auto k = this->addShortToLongMapping(shortKey, std::move(longKey));
	utki::ScopeExit mappingScopeExit([this, shortKey](){
		this->shortToLongMap.erase(shortKey);
	});
	
	this->boolArgs.insert(std::make_pair(std::move(k), std::move(valueHandler)));
	
	mappingScopeExit.reset();
	descriptionScopeExit.reset();
}

std::string Args::description() {
	std::stringstream ss;
	
	for(auto& s : this->argDescriptions){
		ss << s;
	}
	
	return ss.str();
}

namespace{
const std::string longKeyPrefix_c("--");
}

std::vector<std::string> Args::parse(int argc, char** argv) {
	std::vector<std::string> extras;
	
	const unsigned shortKeyArgumentLength = 2;
	
	//first argument is the filename of the executable
	
	for(int i = 1; i < argc; ++i){
		std::string a(argv[i]);
		
		if(a.find(longKeyPrefix_c) == 0 && a.size() > longKeyPrefix_c.size()){
			this->parseLongKeyArgument(a);
		}else if(a.find("-") == 0 && a.size() >= shortKeyArgumentLength){
			auto key = a[1];
			std::string value;
			if(a.size() == shortKeyArgumentLength){
				//value is the next argument
				++i;
				if(i == argc){
					break;
				}
				value = argv[i];
			}else{
				ASSERT(a.size() > shortKeyArgumentLength)
				value = a.substr(shortKeyArgumentLength);
			}
			this->handleShortKey(key, std::move(value));
		}else{
			extras.emplace_back(std::move(a));
		}
	}
	
	return extras;
}

void Args::handleShortKey(char key, std::string&& value) {
	//TODO:
}

void Args::parseLongKeyArgument(const std::string& arg) {
	auto eqPos = arg.find("=");
	if(eqPos != std::string::npos){
		auto value = arg.substr(eqPos + 1);
		auto key = arg.substr(longKeyPrefix_c.size(), eqPos - longKeyPrefix_c.size());
		//TODO:
	}
	//TODO:
}

