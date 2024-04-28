/*
MIT License

Copyright (c) 2018-2023 Ivan Gagis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/* ================ LICENSE END ================ */

#include "parser.hpp"

#include <sstream>

#include <utki/string.hpp>
#include <utki/util.hpp>

using namespace clargs;

void parser::push_back_description(
	char short_key,
	const std::string& long_key,
	std::string description,
	bool is_boolean,
	bool is_value_optional
)
{
	std::stringstream ss;
	ss << "  ";

	if (short_key != '\0') {
		ss << '-' << short_key;
		if (!is_boolean && long_key.empty()) {
			ss << " VALUE";
		}
	}

	if (!long_key.empty()) {
		if (short_key != '\0') {
			ss << ", ";
		} else {
			ss << "    ";
		}

		ss << "--" << long_key;
		if (!is_boolean) {
			if (is_value_optional) {
				ss << "[=VALUE]";
			} else {
				ss << "=VALUE";
			}
		}
	}

	this->key_descriptions.push_back({ss.str(), std::move(description)});
}

void parser::add_argument(
	char short_key,
	std::string long_key,
	std::string description,
	std::function<void(std::string_view)> value_handler,
	std::function<void()> boolean_handler
)
{
	bool is_boolean = !value_handler && boolean_handler;

	this->push_back_description(short_key, long_key, std::move(description), is_boolean, boolean_handler != nullptr);
	utki::scope_exit description_scope_exit([this]() {
		this->key_descriptions.pop_back();
	});

	auto actual_key = this->get_long_key_for_short_key(short_key, std::move(long_key));

	if (this->arguments.find(actual_key) != this->arguments.end()) {
		std::stringstream ss;
		ss << "argument with ";
		ASSERT(!actual_key.empty())
		if (actual_key.front() == ' ') {
			ASSERT(actual_key.size() == 2)
			ss << "short key '" << actual_key[1] << "'";
		} else {
			ss << "long key '" << actual_key << "'";
		}
		ss << " already exists";
		throw std::logic_error(ss.str());
	}

	auto res = this->arguments.insert(
		// NOLINTNEXTLINE(modernize-use-designated-initializers, "needs C++20, while we use C++17")
		std::make_pair(std::move(actual_key), argument_callbacks{std::move(value_handler), std::move(boolean_handler)})
	);
	ASSERT(res.second)

	utki::scope_exit argument_scope_exit([&res, this] {
		this->arguments.erase(res.first);
	});

	// add short to long key mapping if there is a short key
	if (short_key != '\0') {
		auto i = this->short_to_long_map.insert(std::make_pair(short_key, std::string_view(res.first->first)));
		if (!i.second) {
			std::stringstream ss;
			ss << "argument with short key '" << short_key << "' already exists";
			throw std::logic_error(ss.str());
		}
	}

	argument_scope_exit.release();
	description_scope_exit.release();
}

std::string parser::get_long_key_for_short_key(char short_key, std::string&& long_key)
{
	if (long_key.empty() && short_key != '\0') {
		// key name cannot have spaces, so starting a long name with space makes
		// sure it will not clash with another long name of one letter
		std::stringstream ss;
		ss << ' ' << short_key;
		return ss.str();
	}

	return std::move(long_key);
}

std::string parser::description(unsigned keys_width, unsigned width) const
{
	std::stringstream ss;

	auto indentation = std::string(keys_width + 2, ' ');
	for (auto& d : this->key_descriptions) {
		ss << d.key_names;

		if (d.key_names.size() > keys_width) {
			ss << std::endl << indentation;
		} else {
			ss << std::string(keys_width - d.key_names.size(), ' ') << "  ";
		}

		auto lines = utki::word_wrap(d.description, width);

		ASSERT(lines.size() >= 1)

		ss << lines.front() << std::endl;

		for (auto i = std::next(lines.begin()); i != lines.end(); ++i) {
			ss << indentation << (*i) << std::endl;
		}
	}

	return ss.str();
}

namespace {
const std::string long_key_prefix = "--";
const unsigned short_key_argument_size = 2;
} // namespace

std::vector<std::string> parser::parse(int argc, const char* const* argv)
{
	ASSERT(argc >= 1)
	return this->parse(utki::make_span(argv, argc).subspan(1));
}

std::vector<std::string> parser::parse(utki::span<const char* const> args)
{
	std::vector<std::string> ret;

	for (auto i = args.begin(); i != args.end() && !this->stop_parsing_requested; ++i) {
		std::string arg(*i);

		if (this->is_key_parsing_enabled && arg.size() >= long_key_prefix.size() && arg.find(long_key_prefix) == 0) {
			this->parse_long_key_argument(arg);
		} else if (this->is_key_parsing_enabled && arg.size() >= short_key_argument_size && arg[0] == '-') {
			auto h = this->parse_short_keys_batch(arg);

			if (h) {
				// value is the next argument
				++i;
				if (i == args.end()) {
					std::stringstream ss;
					ss << "argument '" << arg.back() << "' requires value";
					throw std::invalid_argument(ss.str());
				}
				(*h)(*i);
			}
		} else {
			if (this->is_key_parsing_enabled && this->subcommand_handler) {
				auto cmd_index = std::distance(args.begin(), i);
				ASSERT(cmd_index >= 0)
				ASSERT(size_t(cmd_index) < args.size())
				++cmd_index;
				this->subcommand_handler( //
					std::string_view(*i),
					args.subspan(cmd_index)
				);
				ASSERT(ret.empty())
				return ret;
			} else {
				if (this->non_key_handler) {
					this->non_key_handler(std::move(arg));
				} else {
					ret.push_back(std::move(arg));
				}
			}
		}
	}

	return ret;
}

void parser::parse_long_key_argument(std::string_view arg)
{
	auto equals_pos = arg.find("=");
	if (equals_pos != std::string::npos) {
		auto value = arg.substr(equals_pos + 1);
		auto key = arg.substr(long_key_prefix.size(), equals_pos - long_key_prefix.size());

		auto i = this->arguments.find(key);
		if (i != this->arguments.end()) {
			if (!i->second.value_handler) {
				std::stringstream ss;
				ss << "key argument '" << std::string(key); // MSVC: no operator<<(std::string_view)
				ss << "' is a boolean argument and cannot have value";
				throw std::invalid_argument(ss.str());
			}
			i->second.value_handler(std::move(value));
			return;
		}
	} else {
		auto key = arg.substr(long_key_prefix.size());
		auto i = this->arguments.find(key);
		if (i != this->arguments.end()) {
			ASSERT(i->second.boolean_handler)
			i->second.boolean_handler();
			return;
		} else if (arg.size() == 2) {
			ASSERT(arg == "--")
			// default handling of '--' argument is disabling key arguments parsing
			this->enable_key_parsing(false);
			return;
		}
	}
	std::stringstream ss;
	ss << "unknown argument: " << std::string(arg); // MSVC: no operator<<(std::string_view)
	throw std::invalid_argument(ss.str());
}

std::function<void(std::string_view)>* parser::parse_short_keys_batch(std::string_view arg)
{
	ASSERT(arg.size() > 1)
	for (unsigned i = 1; i != arg.size(); ++i) {
		auto key = arg[i];

		std::array<char, 2> no_long_key_actual_key = {' '};
		std::string_view actual_key;
		{
			auto iter = this->short_to_long_map.find(key);
			if (iter != this->short_to_long_map.end()) {
				actual_key = iter->second;
			} else {
				no_long_key_actual_key[1] = key; // make key name starting with space
				actual_key = std::string_view(no_long_key_actual_key.data(), no_long_key_actual_key.size());
			}
		}

		auto iter = this->arguments.find(actual_key);
		if (iter == this->arguments.end()) {
			std::stringstream ss;
			ss << "unknown argument: " << std::string(arg); // MSVC: no operator<<(std::string_view)
			throw std::invalid_argument(ss.str());
		}

		auto& h = iter->second;

		if (!h.boolean_handler) {
			ASSERT(h.value_handler)
			++i;
			if (i == arg.size()) {
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

void parser::stop()
{
	this->stop_parsing_requested = true;
}

void parser::add( //
	std::function<void( //
		std::string_view command,
		utki::span<const char* const> args
	)> subcommand_handler
)
{
	if (this->subcommand_handler) {
		throw std::logic_error("subcommand handler is already added");
	}
	this->subcommand_handler = std::move(subcommand_handler);
}
