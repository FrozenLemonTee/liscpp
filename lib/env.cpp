#include "env.h"
#include "builtin.h"


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
}

Env::Env(Env *host, bool is_global) : symbols(), host_env(host) {
    if (is_global){
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
