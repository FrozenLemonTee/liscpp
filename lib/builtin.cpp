#include <iostream>
#include "builtin.h"
#include "error.h"


MalType* operator_plus(const std::vector<MalType *> &args) {
    int64_t result = 0;
    for (const auto& arg: args) {
        auto num = dynamic_cast<MalInt*>(arg);
        if (!num){
            throw argInvalidError("wrong type");
        }
        result += num->get_elem();
    }
    return new MalInt(result);
}

MalType* operator_minus(const std::vector<MalType *> &args) {
    if (args.empty()){
        throw argInvalidError("empty arg list");
    }
    auto first = dynamic_cast<MalInt*>(args[0]);
    if (!first) {
        throw argInvalidError("wrong type");
    }
    int64_t result = first->get_elem();
    for (std::size_t i = 1; i < args.size(); ++i) {
        auto arg = dynamic_cast<MalInt*>(args[i]);
        if (!arg) {
            throw argInvalidError("wrong type");
        }
        int64_t num = arg->get_elem();
        if (num == 0){
            throw valueError("divided by zero");
        }
        result -= num;
    }

    return new MalInt(result);
}

MalType* operator_multiply(const std::vector<MalType *> &args) {
    int64_t result = 1;
    for (const auto& arg: args) {
        auto num = dynamic_cast<MalInt*>(arg);
        if (!num){
            throw argInvalidError("wrong type");
        }
        result *= num->get_elem();
    }
    return new MalInt(result);
}

MalType* operator_divide(const std::vector<MalType *> &args) {
    if (args.empty()){
        throw argInvalidError("empty arg list");
    }
    auto first = dynamic_cast<MalInt*>(args[0]);
    if (!first) {
        throw argInvalidError("wrong type");
    }
    int64_t result = first->get_elem();
    for (std::size_t i = 1; i < args.size(); ++i) {
        auto arg = dynamic_cast<MalInt*>(args[i]);
        if (!arg) {
            throw argInvalidError("wrong type");
        }
        int64_t num = arg->get_elem();
        if (num == 0){
            throw valueError("divided by zero");
        }
        result /= num;
    }

    return new MalInt(result);
}

MalType* prn(const std::vector<MalType*>& args) {
    for (const auto& object: args){
        std::cout << object->to_string() << std::endl;
    }
    return new MalNil;
}

MalType* list(const std::vector<MalType *>& args) {
    return new MalList(args);
}

MalType* is_list(const std::vector<MalType*>& args) {
    if (args.size() != 1){
        throw argInvalidError("expected one arg, given " +
                              std::to_string(args.size()) + " arg(s)");
    }

    return new MalBool(dynamic_cast<MalList*>(args[0]));
}

MalType* is_empty(const std::vector<MalType*>& args) {
    if (args.size() != 1){
        return new MalBool(false);
    }

    auto arg = dynamic_cast<MalList*>(args[0]);
    return new MalBool(arg && arg->get_elem().empty());
}

MalType* count(const std::vector<MalType *>& args) {
    if (args.size() != 1){
        throw argInvalidError("expected one arg, given " +
                              std::to_string(args.size()) + " arg(s)");
    }
    auto arg = dynamic_cast<MalList*>(args[0]);
    if (!arg){
        throw argInvalidError("wrong type");
    }
    return new MalInt(arg->get_elem().size());
}
