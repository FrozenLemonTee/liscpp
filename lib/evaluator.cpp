#include <iostream>
#include "evaluator.h"
#include "env.h"
#include "error.h"


MalType* Evaluator::eval(MalType *input, Env &env) {
    if (MalType* dbg = env.get("DEBUG-EVAL")) {
        auto* b = dynamic_cast<MalBool*>(dbg);
        auto* n = dynamic_cast<MalNil*>(dbg);
        if (!(b && !b->get_elem()) && !n) {
            std::cout << "EVAL: " << input->to_string() << std::endl;
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
        auto& lst_elem = lst->get_elem();
        if (lst_elem.empty()){
            return lst;
        }

        auto first = lst_elem[0];
        auto first_sym = dynamic_cast<MalSymbol*>(first);
        if (first_sym && first_sym->name() == "fn*"){
            if (lst_elem.size() != 3){
                throw syntaxError("expected 2 args, but given " + std::to_string(lst_elem.size() - 1) + "arg(s)");
            }

            auto args_list = dynamic_cast<MalList*>(lst_elem[1]);
            if (!args_list){
                throw typeError("expected an arg list");
            }
            auto function_body = dynamic_cast<MalList*>(lst_elem[2]);
            if (!function_body){
                throw typeError("expected an function body");
            }

            std::vector<std::string> eval_args_list;
            for (const auto e: args_list->get_elem()){
                const auto sym = dynamic_cast<MalSymbol*>(e);
                if (!sym){
                    throw typeError("expected a symbol");
                }
                eval_args_list.emplace_back(sym->name());
            }

            const auto local_env = new Env(&env, false);
            return new MalFunction(args_list, function_body, local_env);
        }

        if (first_sym && first_sym->name() == "do"){
            if (lst_elem.size() == 1){
                return new MalNil;
            }
            for(size_t i = 1; i < lst_elem.size() - 1; i++){
                eval(lst_elem[i], env); // may cause memory leak
            }
            return eval(lst_elem[lst_elem.size() - 1], env);
        }

        if (first_sym && first_sym->name() == "if"){
            if (lst_elem.size() != 3 && lst_elem.size() != 4){
                throw syntaxError("expected 2 or 3 args, but given " + std::to_string(lst_elem.size() - 1) + "arg(s)");
            }
            MalType* cond = eval(lst_elem[1], env);
            const bool truthy = (!dynamic_cast<MalBool*>(cond) || dynamic_cast<MalBool*>(cond)->get_elem())
                          && !dynamic_cast<MalNil*>(cond);
            if (truthy) {
                return eval(lst_elem[2], env);
            }
            return lst_elem.size() == 4 ? eval(lst_elem[3], env) : new MalNil;
        }

        if (first_sym && first_sym->name() == "def!"){
            if (lst_elem.size() != 3){
                throw syntaxError("expected 2 args, but given " + std::to_string(lst_elem.size() - 1) + "arg(s)");
            }
            if (!dynamic_cast<MalSymbol*>(lst_elem[1])){
                throw syntaxError("expected a symbol");
            }

            auto symbol = dynamic_cast<MalSymbol*>(lst_elem[1]);
            auto name = symbol->name();
            MalType* value = eval(lst_elem[2], env);
            env.set(name, value);

            return value;
        }
        if (first_sym && first_sym->name() == "let*"){
            if (lst_elem.size() != 3){
                throw syntaxError("expected 2 args, but given " + std::to_string(lst_elem.size() - 1) + "arg(s)");
            }
            auto binding_list = dynamic_cast<MalList*>(lst_elem[1]);
            auto binding_vector = dynamic_cast<MalVector*>(lst_elem[1]);
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
                auto value = eval(sequence->get_elem()[i + 1], let_env);
                let_env.set(symbol->name(), value);
            }

            return eval(lst_elem[2], let_env)->clone();
        }

        std::vector<MalType*> eval_args;
        for (auto& arg: lst_elem){
            eval_args.emplace_back(eval(arg, env));
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
            eval_args.emplace_back(eval(arg, env));
        }
        return new MalVector(eval_args);
    }

    if (auto map = dynamic_cast<MalMap*>(input); map){
        std::set<MalPair*> eval_args;
        for (auto& e: map->get_elem()){
            eval_args.insert(new MalPair{e->key(), eval(e->value(), env)});
        }
        return new MalMap(eval_args);
    }

    return input;
}
