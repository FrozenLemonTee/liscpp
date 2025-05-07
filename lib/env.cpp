#include "env.h"
#include "builtin.h"
#include "error.h"


void Env::builtin_register() {
    this->add("+", new MalFunction(operator_plus));
    this->add("-", new MalFunction(operator_minus));
    this->add("*", new MalFunction(operator_multiply));
    this->add("/", new MalFunction(operator_divide));
    this->add("prn", new MalFunction(prn));
    this->add("list", new MalFunction(list));
    this->add("list?", new MalFunction(is_list));
    this->add("empty?", new MalFunction(is_empty));
    this->add("count", new MalFunction(count));
    this->add("=", new MalFunction(equal));
    this->add("<", new MalFunction(less));
    this->add("<=", new MalFunction(less_equal));
    this->add(">", new MalFunction(greater));
    this->add(">=", new MalFunction(greater_equal));
}

Env::Env(Env *host, bool is_global) : symbols(), host_env(host), global_(is_global) {
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
    else if (this->host_env != nullptr)
        return this->host_env->get(name);
    else
        return nullptr;
}

void Env::set(const std::string &name, MalType *symbol) {
    this->symbols[name] = symbol;
}

Env::~Env() {
    for (auto& [name, symbol]: this->symbols) {
        delete symbol;
    }
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
    Env* cloned_env = new Env(this->host_env, this->global_);
    for (const auto& [name, symbol]: this->symbols){
        cloned_env->add(name, symbol->clone());
    }
    return cloned_env;
}

Env::Env(Env *host, const std::vector<std::string> &args_list,
         MalFunction::mal_func_args_list_type &params_list) : Env(host, false) {
    if (args_list.size() != params_list.size()){
        throw argInvalidError("expected" + std::to_string(args_list.size()) + " arg(s), given "
                              + std::to_string(params_list.size()) + "arg(s)");
    }
    auto it_args = args_list.begin();
    auto it_params = params_list.begin();
    for (;it_args != args_list.end() && it_params != params_list.end(); it_args++, it_params++){
        this->add(*it_args, *it_params);
    }
}
