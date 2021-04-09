#include <sstream>

#include <utki/string.hpp>

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

	auto key_names = ss.str();

	this->key_descriptions.push_back({
		ss.str(),
		std::move(description)
	});
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
		this->key_descriptions.pop_back();
	});

	auto actual_key = this->add_short_to_long_mapping(short_key, std::move(long_key));

	utki::scope_exit mapping_scope_exit([this, short_key](){
		this->short_to_long_map.erase(short_key);
	});

	auto res = this->arguments.insert(std::make_pair(
			actual_key,
			argument_callbacks{std::move(value_handler), std::move(boolean_handler)}
		));
	
	if(!res.second){
		std::stringstream ss;
		ss << "argument with ";
		ASSERT(!actual_key.empty())
		if(actual_key.front() == ' '){
			ASSERT(actual_key.size() == 2)
			ss << "short key '" << actual_key[1] << "'";
		}else{
			ss << "long key '" << actual_key << "'";
		}
		ss << " already exists";
		throw std::logic_error(ss.str());
	}

	mapping_scope_exit.reset();
	description_scope_exit.reset();
}

std::string parser::add_short_to_long_mapping(char short_key, std::string&& long_key) {
	std::string ret;
	if(long_key.empty() && short_key != '\0'){
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
				throw std::logic_error(ss.str());
			}
		}
	}
	return ret;
}

std::string parser::description(unsigned keys_width, unsigned width)const{
	std::stringstream ss;

	auto indentation = std::string(keys_width + 2, ' ');
	for(auto& d : this->key_descriptions){
		ss << d.key_names;

		if(d.key_names.size() > keys_width){
			ss << std::endl << indentation;
		}else{
			ss << std::string(keys_width - d.key_names.size(), ' ') << "  ";
		}

		auto lines = utki::word_wrap(d.description, width);

		ASSERT(lines.size() >= 1)
		
		ss << lines.front() << std::endl;

		for(auto i = std::next(lines.begin()); i != lines.end(); ++i){
			ss << indentation << (*i) << std::endl;
		}
	}

	return ss.str();
}

namespace{
const std::string long_key_prefix = "--";
const unsigned short_key_argument_size = 2;
}

std::vector<std::string> parser::parse(utki::span<const char* const> args){
	std::vector<std::string> ret;

	ASSERT(!args.empty()) // first argument is the filename of the executable

	for(auto i = std::next(args.begin()); i != args.end(); ++i){
		std::string arg(*i);

		if(this->is_key_parsing_enabled && arg.size() >= long_key_prefix.size() && arg.find(long_key_prefix) == 0){
			this->parse_long_key_argument(arg);
		}else if(this->is_key_parsing_enabled && arg.size() >= short_key_argument_size && arg[0] == '-'){
			auto h = this->parse_short_keys_batch(arg);

			if(h){
				// value is the next argument
				++i;
				if(i == args.end()){
					std::stringstream ss;
					ss << "argument '" << arg.back() << "' requires value";
					throw std::invalid_argument(ss.str());
				}
				(*h)(*i);
			}
		}else{
			if(this->is_key_parsing_enabled && this->subcommand_handler){
				this->subcommand_handler(args.subspan(std::distance(args.begin(), i)));
				ASSERT(ret.empty())
				return ret;
			}else{
				if(this->non_key_handler){
					this->non_key_handler(std::move(arg));
				}else{
					ret.push_back(std::move(arg));
				}
			}
		}
	}

	return ret;
}

void parser::parse_long_key_argument(const std::string& arg){
	auto equals_pos = arg.find("=");
	if(equals_pos != std::string::npos){
		auto value = arg.substr(equals_pos + 1);
		auto key = arg.substr(long_key_prefix.size(), equals_pos - long_key_prefix.size());

		auto i = this->arguments.find(key);
		if(i != this->arguments.end()){
			if(!i->second.value_handler){
				std::stringstream ss;
				ss << "key argument '" << key << "' is a boolean argument and cannot have value";
				throw std::invalid_argument(ss.str());
			}
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
		}else if(arg.size() == 2){
			ASSERT(arg == "--")
			// default handling of '--' argument is disabling key arguments parsing
			this->enable_key_parsing(false);
			return;
		}
	}
	std::stringstream ss;
	ss << "unknown argument: " << arg;
	throw std::invalid_argument(ss.str());
}

std::function<void(std::string&&)>* parser::parse_short_keys_batch(const std::string& arg){
	ASSERT(arg.size() > 1)
	for(unsigned i = 1; i != arg.size(); ++i){
		auto key = arg[i];

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
			ss << "unknown argument: " << arg;
			throw std::invalid_argument(ss.str());
		}

		auto& h = iter->second;

		if(!h.boolean_handler){
			ASSERT(h.value_handler)
			++i;
			if(i == arg.size()){
				return &h.value_handler;
			}
			ASSERT(i < arg.size())
			h.value_handler(arg.substr(i));
			break;
		}
		ASSERT(!iter->second.value_handler)
		iter->second.boolean_handler();
	}
	return nullptr;
}
