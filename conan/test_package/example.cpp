#include <clargs/parser.hpp>

int main(int argc, const char** argv){
	clargs::parser p;

	bool help = false;

	p.add("help", "show help information", [&help](){help = true;});

	std::cout << p.description() << '\n';
}
