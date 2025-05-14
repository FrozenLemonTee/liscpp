#include <string>
#include <utility>
#include "evaluator.h"
#include "error.h"
#include "iostream"
#include "reader.h"
#include "types.h"
#include "printer.h"
#include "env.h"
#include "builtin.h"
#include <cstdlib>


MalType* READ(std::string input){
    return Reader::read_str(std::move(input));
}

MalType* EVAL(MalType* input, Env& env) {
    return Evaluator::eval(input, &env);
}

std::string PRINT(const MalType* input) {
    return Printer::pr_str(input, true);
}

void file_exec(char** argv){
    auto path = std::string(argv[1]);
    try {
        load_file({new MalString(path)}, false);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }
}

void repl(Env& global_env){
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
}

int main(int argc, char** argv){
    Env global_env;
    std::vector<MalType*> argv_list;
    for (int i = 2; i < argc; ++i) {
        argv_list.push_back(new MalString(argv[i]));
    }
    global_env.set("*ARGV*", new MalList(argv_list));
    Evaluator::set_env(&global_env);

    if (argc >= 2){
        file_exec(argv);
    } else{
        repl(global_env);
    }

    return 0;
}
