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
	
	ss << "  " << description;
	
	if(description.rbegin() != description.rend() && *description.rbegin() != '.'){
		ss << ".";
	}
	
	ss << std::endl;
	
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
//		TRACE(<< "k = " << k << std::endl)
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
		std::string arg(argv[i]);
		
		if(arg.find(longKeyPrefix_c) == 0 && arg.size() > longKeyPrefix_c.size()){
			this->parseLongKeyArgument(arg);
		}else if(arg.find("-") == 0 && arg.size() >= shortKeyArgumentLength){
			auto key = arg[1];
			
			std::string actualKey;
			{
				auto iter = this->shortToLongMap.find(key);
				if(iter != this->shortToLongMap.end()){
					actualKey = iter->second;
				}else{
					actualKey = std::string(1, key);
				}
			}

			{
				auto iter = this->boolArgs.find(actualKey);
				if(iter != this->boolArgs.end()){
					ASSERT(iter->second)
					iter->second();
					continue;
				}
			}

			auto iter = this->valueArgs.find(actualKey);
			if(iter == this->valueArgs.end()){
				std::stringstream ss;
				ss << "Unknown argument: " << arg;
				throw UnknownArgumentExc(ss.str());
			}
			ASSERT(iter->second)
			
			std::string value;
			if(arg.size() == shortKeyArgumentLength){
				//value is the next argument
				++i;
				if(i == argc){
					break;
				}
				value = argv[i];
			}else{
				ASSERT(arg.size() > shortKeyArgumentLength)
				value = arg.substr(shortKeyArgumentLength);
			}
			iter->second(std::move(value));
		}else{
			extras.emplace_back(std::move(arg));
		}
	}
	
	return extras;
}


void Args::parseLongKeyArgument(const std::string& arg) {
	auto eqPos = arg.find("=");
	if(eqPos != std::string::npos){
		auto value = arg.substr(eqPos + 1);
		auto key = arg.substr(longKeyPrefix_c.size(), eqPos - longKeyPrefix_c.size());
		
		auto i = this->valueArgs.find(key);
		if(i != this->valueArgs.end()){
			ASSERT(i->second)
			i->second(std::move(value));
			return;
		}
	}else{
		auto key = arg.substr(longKeyPrefix_c.size());
		auto i = this->boolArgs.find(key);
		if(i != this->boolArgs.end()){
			ASSERT(i->second)
			i->second();
			return;
		}
	}
	std::stringstream ss;
	ss << "Unknown argument: " << arg;
	throw UnknownArgumentExc(ss.str());
}


void Args::add(
		char shortKey,
		std::string&& longKey,
		std::string&& description,
		std::function<void(long)>&& valueHandler
	)
{
	//TODO:
}

void Args::add(
		char shortKey,
		std::string&& longKey,
		std::string&& description,
		std::function<void(double)>&& valueHandler
	)
{
	//TODO:
}
