#include <iostream>
#include <string>

#include "parser.h"

int main(int argc, char const * argv[])
{
    std::string input;
    std::cout << "> ";
    std::getline(std::cin, input);

    try {
        ast::NodePtr ast = ast::parser::parse(input);
        std::string outputFile;

        std::cout << "Filename of .dot: ";
        std::getline(std::cin, outputFile);
        dumpToGraphviz(outputFile, ast);
    } catch (const std::exception& /* ex */) {
        // Temporary solution
        throw std::runtime_error("Parse error!");
    }
}
