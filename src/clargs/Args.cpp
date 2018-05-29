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
		ss << ", --" << longKey;
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
	
	ss << "  " << description << std::endl;
	
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
