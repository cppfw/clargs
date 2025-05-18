#include <clargs/parser.hpp>

int main(int argc, const char** argv){
    clargs::parser p;

    p.add('a', "aaa", "bla bla", [](){});

    std::cout << "desc = " << p.description() << std::endl;

    return 0;
}
