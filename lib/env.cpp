#include "env.h"
#include "builtin.h"


void Env::builtin_register() {
    this->add("+", new MalFunction(operator_plus));
    this->add("-", new MalFunction(operator_minus));
    this->add("*", new MalFunction(operator_multiply));
    this->add("/", new MalFunction(operator_divide));
}

Env::Env(Env* env) : symbols(), host_env(env) {
    this->builtin_register();
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
