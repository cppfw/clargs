#include <sstream>

#include <utki/util.hpp>

#include "parser.hpp"

using namespace clargs;


template <bool b> void parser::add_description(char shortKey, const std::string& longKey, std::string&& description) {
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

template <bool b> void parser::add_argument(
		char shortKey,
		std::string&& longKey,
		std::string&& description,
		std::function<void(std::string&& value)>&& valueHandler
	)
{
	this->add_description<b>(shortKey, longKey, std::move(description));
	utki::ScopeExit descriptionScopeExit([this](){this->argDescriptions.pop_back();});

	auto k = this->add_short_to_long_mapping(shortKey, std::move(longKey));
	utki::ScopeExit mappingScopeExit([this, shortKey](){
		this->shortToLongMap.erase(shortKey);
	});

	if(b){
		this->boolArgs.insert(std::make_pair(std::move(k), std::move(valueHandler)));
	}else{
		this->valueArgs.insert(std::make_pair(std::move(k), std::move(valueHandler)));
	}

	mappingScopeExit.reset();
	descriptionScopeExit.reset();
}

std::string parser::add_short_to_long_mapping(char shortKey, std::string&& longKey) {
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

std::string parser::description() {
	std::stringstream ss;

	for(auto& s : this->argDescriptions){
		ss << s;
	}

	return ss.str();
}

namespace{
const std::string longKeyPrefix_c("--");
}

std::vector<std::string> parser::parse(int argc, char** argv) {
	std::vector<std::string> extras;

	const unsigned shortKeyArgumentLength = 2;

	//first argument is the filename of the executable

	for(int i = 1; i < argc; ++i){
		std::string arg(argv[i]);

		if(arg.find(longKeyPrefix_c) == 0 && arg.size() > longKeyPrefix_c.size()){
			this->parse_long_key_argument(arg);
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
					iter->second(std::string());
					continue;
				}
			}

			auto iter = this->valueArgs.find(actualKey);
			if(iter == this->valueArgs.end()){
				std::stringstream ss;
				ss << "Unknown argument: " << arg;
				throw unknown_argument_exception(ss.str());
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


void parser::parse_long_key_argument(const std::string& arg) {
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
			i->second(std::string());
			return;
		}
	}
	std::stringstream ss;
	ss << "Unknown argument: " << arg;
	throw unknown_argument_exception(ss.str());
}

void parser::add(char shortKey, std::string&& longKey, std::string&& description, std::function<void(std::string&&)>&& valueHandler) {
	this->add_argument<false>(shortKey, std::move(longKey), std::move(description), std::move(valueHandler));
}


void parser::add(
		char shortKey,
		std::string&& longKey,
		std::string&& description,
		std::function<void()>&& valueHandler
	)
{
	this->add_argument<true>(
			shortKey,
			std::move(longKey),
			std::move(description),
			[valueHandler](std::string&&){ //TODO: use move capture when C++14 is taken into use
				valueHandler();
			}
		);
}
