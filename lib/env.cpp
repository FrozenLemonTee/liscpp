#include "env.h"
#include "builtin.h"
#include "error.h"


void Env::builtin_register() {
    this->add("*ARGV*", new MalList({}));
    this->add("+", new MalFunction(operator_plus));
    this->add("-", new MalFunction(operator_minus));
    this->add("*", new MalFunction(operator_multiply));
    this->add("/", new MalFunction(operator_divide));
    this->add("str", new MalFunction(str));
    this->add("pr-str", new MalFunction(pr_str));
    this->add("prn", new MalFunction(prn));
    this->add("println", new MalFunction(println));
    this->add("list", new MalFunction(list));
    this->add("list?", new MalFunction(is_list));
    this->add("empty?", new MalFunction(is_empty));
    this->add("count", new MalFunction(count));
    this->add("=", new MalFunction(equal));
    this->add("<", new MalFunction(less));
    this->add("<=", new MalFunction(less_equal));
    this->add(">", new MalFunction(greater));
    this->add(">=", new MalFunction(greater_equal));
    this->add("not", new MalFunction(not_func));
    this->add("read-string", new MalFunction(read_string));
    this->add("slurp", new MalFunction(slurp));
    this->add("eval", new MalFunction(evals));
    this->add("load-file", new MalFunction(load_file_repl));
    this->add("atom", new MalFunction(atom));
    this->add("atom?", new MalFunction(is_atom));
    this->add("deref", new MalFunction(deref));
    this->add("reset!", new MalFunction(reset));
    this->add("swap!", new MalFunction(swap));
    this->add("cons", new MalFunction(cons));
    this->add("concat", new MalFunction(concat));
    this->add("vec", new MalFunction(vec));
}

Env::Env(Env *host, const bool is_global)
    : global_(is_global), host_env(host) {
    if (this->global_){
        this->builtin_register();
    }
}

void Env::add(const std::string& name, MalType *symbol) {
    this->symbols.insert({name, symbol});
}

MalType *Env::get(const std::string &name) {
    if (this->symbols.contains(name))
        return this->symbols[name];
    if (this->host_env != nullptr)
        return this->host_env->get(name);
    return nullptr;
}

void Env::set(const std::string &name, MalType *symbol) {
    this->symbols[name] = symbol;
}

Env* Env::find(const std::string &name) {
    if (this->symbols.contains(name))
        return this;
    if (this->host_env){
        return this->host_env->find(name);
    }
    return nullptr;
}

Env* Env::clone() const {
    const auto cloned_env = new Env(this->host_env, this->global_);
    for (const auto& [name, symbol]: this->symbols){
        cloned_env->add(name, symbol->clone());
    }
    return cloned_env;
}

Env::Env(Env *host, const std::vector<std::string> &args_list,
         MalFunction::mal_func_args_list_type &params_list) : Env(host, false) {
    if (const auto it = std::ranges::find(args_list, "&"); it != args_list.end()) {
        if (std::distance(it, args_list.end()) != 2) {
            throw syntaxError("malfunctioning & param usage");
        }

        const std::size_t fixed_arity = std::distance(args_list.begin(), it);
        if (params_list.size() < fixed_arity) {
            throw argInvalidError("too few arguments");
        }

        for (std::size_t i = 0; i < fixed_arity; ++i) {
            this->set(args_list[i], params_list[i]);
        }

        const auto rest_begin = std::next(params_list.begin(), static_cast<std::vector<MalType*>::difference_type>(fixed_arity));
        const std::vector<MalType*> rest{rest_begin, params_list.end()};
        this->set(*(it + 1), new MalList(rest));
    } else {
        if (args_list.size() != params_list.size()) {
            throw argInvalidError("expected " + std::to_string(args_list.size()) +
                                  " arg(s), given " + std::to_string(params_list.size()) + " arg(s)");
        }

        auto it_args = args_list.begin();
        for (auto it_params = params_list.begin(); it_args != args_list.end(); ++it_args, ++it_params) {
            this->add(*it_args, *it_params);
        }
    }
}
