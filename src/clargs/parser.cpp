#include <sstream>

#include <utki/util.hpp>

#include "parser.hpp"

using namespace clargs;

void parser::push_back_description(
		char short_key,
		const std::string& long_key,
		std::string&& description,
		bool is_boolean,
		bool is_value_optional
	)
{
	std::stringstream ss;
	ss << "  ";

	if(short_key != '\0'){
		ss << '-' << short_key;
		if(!is_boolean && long_key.empty()){
			ss << " VALUE";
		}
	}

	if(!long_key.empty()){
		if(short_key != '\0'){
			ss << ", ";
		}else{
			ss << "    ";
		}
		
		ss << "--" << long_key;
		if(!is_boolean){
			if(is_value_optional){
				ss << "[=VALUE]";
			}else{
				ss << "=VALUE";
			}
		}
	}

	const unsigned description_newline_threshold = 38;

	unsigned keys_length = ss.tellp();
	if(keys_length > description_newline_threshold){
		ss << std::endl << std::string(description_newline_threshold, ' ');
	}else{
		ss << std::string(description_newline_threshold - keys_length, ' ');
	}

	ss << "  " << description;

	ss << std::endl;

	this->argument_descriptions.push_back(ss.str());
}

void parser::add_argument(
		char short_key,
		std::string&& long_key,
		std::string&& description,
		std::function<void(std::string&&)>&& value_handler,
		std::function<void()>&& boolean_handler
	)
{
	bool is_boolean = !value_handler && boolean_handler;

	this->push_back_description(
			short_key,
			long_key,
			std::move(description),
			is_boolean,
			boolean_handler != nullptr
		);
	utki::scope_exit description_scope_exit([this](){
		this->argument_descriptions.pop_back();
	});

	auto k = this->add_short_to_long_mapping(short_key, std::move(long_key));
	utki::scope_exit mapping_scope_exit([this, short_key](){
		this->short_to_long_map.erase(short_key);
	});

	auto res = this->arguments.insert(std::make_pair(
			std::move(k),
			argument_callbacks{std::move(value_handler), std::move(boolean_handler)}
		));
	
	if(!res.second){
		std::stringstream ss;
		ss << "argument with ";
		if(!k.empty() && k.front() == ' '){
			ASSERT(k.size() == 2)
			ss << "short key '" << k[1] << "'";
		}else{
			ss << "long key '" << k << "'";
		}
		ss << " already exists";
		throw utki::invalid_state(ss.str());
	}

	mapping_scope_exit.reset();
	description_scope_exit.reset();
}

std::string parser::add_short_to_long_mapping(char short_key, std::string&& long_key) {
	std::string ret;
	if(long_key.empty()){
		if(short_key == '\0'){
			throw std::invalid_argument("both short and long key names are empty");
		}
		// key name cannot have spaces, so starting a long name with space makes
		// sure it will not clash with another long name of one letter
		std::stringstream ss;
		ss << ' ' << short_key;
		ret = ss.str();
	}else{
		ret = std::move(long_key);

		if(short_key != 0){
			auto i = this->short_to_long_map.insert(std::make_pair(short_key, ret));
			if(!i.second){
				std::stringstream ss;
				ss << "argument with short key '" << short_key << "' already exists";
				throw utki::invalid_state(ss.str());
			}
		}
	}
	return ret;
}

std::string parser::description() {
	std::stringstream ss;

	for(auto& s : this->argument_descriptions){
		ss << s;
	}

	return ss.str();
}

namespace{
const std::string long_key_prefix("--");
}

std::vector<std::string> parser::parse(int argc, char** argv) {
	std::vector<std::string> extras;

	const unsigned short_key_argument_size = 2;

	// first argument is the filename of the executable

	for(int i = 1; i < argc; ++i){
		std::string arg(argv[i]);

		if(arg.size() >= long_key_prefix.size() && arg.find(long_key_prefix) == 0){
			this->parse_long_key_argument(arg);
		}else if(arg.size() >= short_key_argument_size && arg.find("-") == 0){
			auto key = arg[1];

			std::string actual_key;
			{
				auto iter = this->short_to_long_map.find(key);
				if(iter != this->short_to_long_map.end()){
					actual_key = iter->second;
				}else{
					actual_key = std::string(1, ' ') + key;
				}
			}

			auto iter = this->arguments.find(actual_key);
			if(iter == this->arguments.end()){
				std::stringstream ss;
				ss << "Unknown argument: " << arg;
				throw std::invalid_argument(ss.str());
			}

			if(iter->second.boolean_handler){
				ASSERT(!iter->second.value)
				iter->second.boolean_handler();
				continue;
			}
			ASSERT(iter->second.value_handler)
			
			std::string value;
			if(arg.size() == short_key_argument_size){
				// value is the next argument
				++i;
				if(i == argc){
					break;
				}
				value = argv[i];
			}else{
				ASSERT(arg.size() > short_key_argument_size)
				value = arg.substr(short_key_argument_size);
			}
			iter->second.value_handler(std::move(value));
		}else{
			extras.emplace_back(std::move(arg));
		}
	}

	return extras;
}


void parser::parse_long_key_argument(const std::string& arg) {
	auto equals_pos = arg.find("=");
	if(equals_pos != std::string::npos){
		auto value = arg.substr(equals_pos + 1);
		auto key = arg.substr(long_key_prefix.size(), equals_pos - long_key_prefix.size());

		auto i = this->arguments.find(key);
		if(i != this->arguments.end()){
			ASSERT(i->second.value_handler)
			i->second.value_handler(std::move(value));
			return;
		}
	}else{
		auto key = arg.substr(long_key_prefix.size());
		auto i = this->arguments.find(key);
		if(i != this->arguments.end()){
			ASSERT(i->second.boolean_handler)
			i->second.boolean_handler();
			return;
		}
	}
	std::stringstream ss;
	ss << "Unknown argument: " << arg;
	throw std::invalid_argument(ss.str());
}

void parser::add(
		char short_key,
		std::string&& long_key,
		std::string&& description,
		std::function<void(std::string&&)>&& value_handler,
		std::function<void()>&& boolean_handler
	)
{
	this->add_argument(
			short_key,
			std::move(long_key),
			std::move(description),
			std::move(value_handler),
			std::move(boolean_handler)
		);
}

//TODO: move to hpp
void parser::add(
		char short_key,
		std::string&& long_key,
		std::string&& description,
		std::function<void()>&& value_handler
	)
{
	this->add_argument(
			short_key,
			std::move(long_key),
			std::move(description),
			nullptr,
			std::move(value_handler)
		);
}
