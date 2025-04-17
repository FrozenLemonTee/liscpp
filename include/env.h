#ifndef ENV_H
#define ENV_H

#include "map"
#include "string"
#include "types.h"


class Env {
    std::map<std::string, MalType*> symbols;

    void builtin_register();
public:
    explicit Env();
    void add(const std::string& name, MalType* symbol);
    MalType* get(const std::string& name);
    ~Env();
};

#endif //ENV_H
