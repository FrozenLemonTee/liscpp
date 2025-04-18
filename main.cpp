#include <string>
#include <utility>

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
    if (auto sym = dynamic_cast<MalSymbol*>(input); sym){
        MalType* opt = env.get(sym->name());
        if (!opt){
            throw typeError("Unknown symbol " + std::string("\"") +  sym->name() + std::string("\""));
        }
        return opt;
    }

    if (auto lst = dynamic_cast<MalList*>(input); lst){
        if (lst->get_elem().empty()){
            return lst;
        }

        std::vector<MalType*> eval_args;
        for (auto& arg: lst->get_elem()){
            eval_args.emplace_back(EVAL(arg, env));
        }

        auto fn = dynamic_cast<MalFunction*>(eval_args[0]);
        if (!fn){
            throw typeError(eval_args[0]->to_string() + " is not a function");
        }
        return fn->apply({eval_args.begin() + 1, eval_args.end()});
    }

    if (auto vec = dynamic_cast<MalVector*>(input); vec){
        std::vector<MalType*> eval_args;
        for (auto& arg: vec->get_elem()){
            eval_args.emplace_back(EVAL(arg, env));
        }
        return new MalVector(eval_args);
    }

    if (auto map = dynamic_cast<MalMap*>(input); map){
        std::map<MalType*, MalType*> eval_args;
        for (auto& [key, value]: map->get_elem()){
            eval_args.insert({key, EVAL(value, env)});
        }
        return new MalMap(eval_args);
    }

    return input;
}

std::string PRINT(MalType* input) {
    return Printer::pr_str(input);
}

int main(){
    Env env;

    while(true){
        std::cout << "user> ";
        std::string input;
        if (!std::getline(std::cin, input)) {
            std::cout << std::endl;
            break;
        }try {
            std::cout << PRINT(EVAL(READ(input), env)) << std::endl;
        }catch(const syntaxError& e) {
            std::cout << e.what() << std::endl;
        }catch(const typeError& e){
            std::cout << e.what() << std::endl;
        }
    }

    return 0;
}
