#include <iostream>
#include "evaluator.h"
#include "env.h"
#include "error.h"

Env* Evaluator::repl_env = nullptr;

MalType* Evaluator::eval(MalType *input, Env* env) {
    while (true){
        if (MalType* dbg = env->get("DEBUG-EVAL")) {
            auto* b = dynamic_cast<MalBool*>(dbg);
            if (const auto* n = dynamic_cast<MalNil*>(dbg); !(b && !b->get_elem()) && !n) {
                std::cout << "EVAL: " << input->to_string(true) << std::endl;
            }
        }

        if (const auto sym = dynamic_cast<MalSymbol*>(input); sym){
            MalType* opt = env->get(sym->name());
            if (!opt){
                throw typeError("'" + sym->name() + "'" + " not found.");
            }
            return opt;
        }

        if (const auto lst = dynamic_cast<MalList*>(input); lst){
            const auto& lst_elem = lst->get_elem();
            if (lst_elem.empty()){
                return lst;
            }

            const auto first = lst_elem[0];
            const auto first_sym = dynamic_cast<MalSymbol*>(first);
            if (first_sym && first_sym->name() == "fn*"){
                if (lst_elem.size() != 3){
                    throw syntaxError("expected 2 args, but given " + std::to_string(lst_elem.size() - 1) + "arg(s)");
                }

                const auto args_list = dynamic_cast<MalSequence*>(lst_elem[1]);
                if (!args_list){
                    throw typeError("expected an arg list");
                }
                MalType* function_body = lst_elem[2];
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

                const auto local_env = new Env(env, false);
                return new MalFunction(args_list, function_body, local_env);
            }

            if (first_sym && first_sym->name() == "do"){
                if (lst_elem.size() == 1){
                    return new MalNil;
                }
                for(size_t i = 1; i < lst_elem.size() - 1; i++){
                    eval(lst_elem[i], env);
                }
                input = lst_elem.back();
                continue;
            }

            if (first_sym && first_sym->name() == "if"){
                if (lst_elem.size() != 3 && lst_elem.size() != 4){
                    throw syntaxError("expected 2 or 3 args, but given " + std::to_string(lst_elem.size() - 1) + "arg(s)");
                }
                MalType* cond = eval(lst_elem[1], env);
                const bool truthy = (!dynamic_cast<MalBool*>(cond) || dynamic_cast<MalBool*>(cond)->get_elem())
                                    && !dynamic_cast<MalNil*>(cond);
                if (truthy) {
                    input = lst_elem[2];
                } else if (lst_elem.size() == 4){
                    input = lst_elem[3];
                } else{
                    return new MalNil;
                }
                continue;
            }

            if (first_sym && first_sym->name() == "def!"){
                if (lst_elem.size() != 3){
                    throw syntaxError("expected 2 args, but given " + std::to_string(lst_elem.size() - 1) + "arg(s)");
                }
                if (!dynamic_cast<MalSymbol*>(lst_elem[1])){
                    throw syntaxError("expected a symbol");
                }

                const auto symbol = dynamic_cast<MalSymbol*>(lst_elem[1]);
                const auto name = symbol->name();
                MalType* value = eval(lst_elem[2], env);
                env->set(name, value);

                return value;
            }
            if (first_sym && first_sym->name() == "let*"){
                if (lst_elem.size() != 3){
                    throw syntaxError("expected 2 args, but given " + std::to_string(lst_elem.size() - 1) + "arg(s)");
                }

                const auto binding_sequence = dynamic_cast<MalSequence*>(lst_elem[1]);
                if (!binding_sequence){
                    throw syntaxError("expected a list or a vector for binding-list of let*");
                }

                if (binding_sequence->get_elem().size() % 2 != 0){
                    throw syntaxError("expected a value for a symbol to bind");
                }

                Env* let_env = new Env(env, false);
                for (std::size_t i = 0; i < binding_sequence->get_elem().size(); i += 2){
                    const auto symbol = dynamic_cast<MalSymbol*>(binding_sequence->get_elem()[i]);
                    if (!symbol) throw syntaxError("let* binding name must be symbol");
                    const auto value = eval(binding_sequence->get_elem()[i + 1], let_env);
                    let_env->set(symbol->name(), value);
                }

                input = lst_elem[2];
                env = let_env;
                continue;
            }

            std::vector<MalType*> eval_params;
            for (auto& arg: lst_elem){
                eval_params.emplace_back(eval(arg, env));
            }

            const auto fn = dynamic_cast<MalFunction*>(eval_params[0]);
            if (!fn){
                throw typeError(eval_params[0]->to_string(true) + " is not a function");
            }
            MalFunction::mal_func_args_list_type fn_params_list = std::vector(eval_params.begin() + 1, eval_params.end());
            if (fn->is_builtin_func()){
                return fn->apply(fn_params_list);
            }
            const auto& args_list_elems = fn->get_args_list()->get_elem();
            const auto size = args_list_elems.size();
            std::vector<std::string> args_names(size);
            for (std::size_t i = 0; i < size; ++i){
                const auto sym = dynamic_cast<MalSymbol*>(args_list_elems[i]);
                if (!sym) {
                    throw typeError("fn* parameters must be symbols");
                }
                args_names[i] = sym->name();
            }

            auto fn_env = fn->get_env();
            auto fn_body = fn->get_body();
            const auto local_env = new Env(fn_env, args_names, fn_params_list);
            input = fn_body;
            env = local_env;
            continue;
        }

        if (const auto vec = dynamic_cast<MalVector*>(input); vec){
            std::vector<MalType*> eval_args;
            for (const auto& arg: vec->get_elem()){
                eval_args.emplace_back(eval(arg, env));
            }
            return new MalVector(eval_args);
        }

        if (const auto map = dynamic_cast<MalMap*>(input); map){
            std::set<MalPair*> eval_args;
            for (auto& e: map->get_elem()){
                eval_args.insert(new MalPair{e->key(), eval(e->value(), env)});
            }
            return new MalMap(eval_args);
        }

        return input;
    }
}

MalType* Evaluator::eval(MalType* input) {
    if (!repl_env) {
        throw REPLError("REPL environment not set.");
    }

    return eval(input, repl_env);
}

void Evaluator::set_env(Env *env) {
    repl_env = env;
}
