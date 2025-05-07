#ifndef ENV_H
#define ENV_H

#include "map"
#include "string"
#include "types.h"


class Env {
    std::map<std::string, MalType*> symbols;
    bool global_;
    Env* host_env;

    void builtin_register();
public:
    explicit Env(Env *host = nullptr, bool is_global = true);
    Env(Env* host, const std::vector<std::string> &args_list, MalFunction::mal_func_args_list_type& params_list);
    void add(const std::string& name, MalType* symbol);
    MalType* get(const std::string& name);
    Env* find(const std::string& name);
    void set(const std::string& name, MalType* symbol);
    [[nodiscard]] Env* clone() const;
    ~Env();
};

#endif //ENV_H
