#ifndef ENV_H
#define ENV_H

#include "map"
#include "string"
#include "types.h"


class Env {
    std::map<std::string, MalType*> symbols;
    Env* host_env;

    void builtin_register();
public:
    explicit Env(Env* env = nullptr);
    void add(const std::string& name, MalType* symbol);
    MalType* get(const std::string& name);
    void set(const std::string& name, MalType* symbol);
    ~Env();
};

#endif //ENV_H
