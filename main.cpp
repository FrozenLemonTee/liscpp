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
    if (MalType* dbg = env.get("DEBUG-EVAL")) {
        auto* b = dynamic_cast<MalBool*>(dbg);
        auto* n = dynamic_cast<MalNil*>(dbg);
        if (!(b && !b->get_elem()) && !n) {
            std::cout << "EVAL: " << Printer::pr_str(input) << std::endl;
        }
    }

    if (auto sym = dynamic_cast<MalSymbol*>(input); sym){
        MalType* opt = env.get(sym->name());
        if (!opt){
            throw typeError("'" + sym->name() + "'" + " not found.");
        }
        return opt;
    }

    if (auto lst = dynamic_cast<MalList*>(input); lst){
        if (lst->get_elem().empty()){
            return lst;
        }

        auto first = lst->get_elem()[0];
        if (first && dynamic_cast<MalSymbol*>(first)->name() == "def!"){
            if (lst->get_elem().size() != 3){
                throw syntaxError("expected 2 args, but given " + std::to_string(lst->get_elem().size() - 1) + "arg(s)");
            }
            if (!dynamic_cast<MalSymbol*>(lst->get_elem()[1])){
                throw syntaxError("expected a symbol");
            }

            auto symbol = dynamic_cast<MalSymbol*>(lst->get_elem()[1]);
            auto name = symbol->name();
            MalType* value = EVAL(lst->get_elem()[2], env);
            env.set(name, value);

            return value;
        }
        if (first && dynamic_cast<MalSymbol*>(first)->name() == "let*"){
            if (lst->get_elem().size() != 3){
                throw syntaxError("expected 2 args, but given " + std::to_string(lst->get_elem().size() - 1) + "arg(s)");
            }
            auto binding_list = dynamic_cast<MalList*>(lst->get_elem()[1]);
            auto binding_vector = dynamic_cast<MalVector*>(lst->get_elem()[1]);
            if (!binding_list && !binding_vector){
                throw syntaxError("expected a list or a vector for binding-list of let*");
            }

            if (binding_list && binding_list->get_elem().size() % 2 != 0){
                throw syntaxError("expected a value for a symbol to bind");
            }
            if (binding_vector && binding_vector->get_elem().size() % 2 != 0){
                throw syntaxError("expected a value for a symbol to bind");
            }

            Env let_env(&env, false);
            MalSequence* sequence = binding_list ? static_cast<MalSequence*>(binding_list) : static_cast<MalSequence*>(binding_vector);
            for (std::size_t i = 0; i < sequence->get_elem().size(); i += 2){
                auto symbol = dynamic_cast<MalSymbol*>(sequence->get_elem()[i]);
                if (!symbol) throw syntaxError("let* binding name must be symbol");
                auto value = EVAL(sequence->get_elem()[i + 1], let_env);
                let_env.set(symbol->name(), value);
            }

            return EVAL(lst->get_elem()[2], let_env)->clone();
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
        std::set<MalPair*> eval_args;
        for (auto& e: map->get_elem()){
            eval_args.insert(new MalPair{e->key(), EVAL(e->value(), env)});
        }
        return new MalMap(eval_args);
    }

    return input;
}

std::string PRINT(MalType* input) {
    return Printer::pr_str(input);
}

int main(){
    Env global_env;

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
