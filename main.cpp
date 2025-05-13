#include <string>
#include <utility>
#include "evaluator.h"
#include "error.h"
#include "iostream"
#include "reader.h"
#include "types.h"
#include "printer.h"
#include "env.h"


MalType* READ(std::string input){
    return Reader::read_str(std::move(input));
}

MalType* EVAL(MalType* input, Env& env) {
    return Evaluator::eval(input, &env);
}

std::string PRINT(const MalType* input) {
    return Printer::pr_str(input, true);
}

int main(){
    Env global_env;
    Evaluator::set_env(&global_env);
    while(true){
        std::cout << "user> ";
        std::string input;
        if (!std::getline(std::cin, input)) {
            std::cout << std::endl;
            break;
        }try {
            std::cout << PRINT(EVAL(READ(input), global_env)) << std::endl;
        }catch(const syntaxError& e) {
            std::cout << e.what() << std::endl;
        }catch(const typeError& e){
            std::cout << e.what() << std::endl;
        }
    }

    return 0;
}
