#include <string>
#include <utility>

#include "error.h"
#include "iostream"
#include "reader.h"
#include "types.h"
#include "printer.h"

MalType* READ(std::string input){
    return Reader::read_str(std::move(input));
}

MalType* EVAL(MalType* input) {
    return input;
}

std::string PRINT(MalType* input) {
    return Printer::pr_str(input);
}

int main(){
    while(true){
        std::cout << "user> ";
        std::string input;
        if (!std::getline(std::cin, input)) {
            std::cout << std::endl;
            break;
        }try {
            std::cout << PRINT(EVAL(READ(input))) << std::endl;
        }catch(const syntaxError&) {
            std::cout << std::endl;
        }
    }

    return 0;
}
