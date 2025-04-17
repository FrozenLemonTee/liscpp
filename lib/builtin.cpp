#include "builtin.h"
#include "error.h"


MalType* operator_plus(const std::vector<MalType *> &args) {
    int64_t result = 0;
    for (const auto& arg: args) {
        auto num = dynamic_cast<MalInt*>(arg);
        if (!num){
            throw typeError("wrong type");
        }
        result += num->get_elem();
    }
    return new MalInt(result);
}

MalType* operator_minus(const std::vector<MalType *> &args) {
    int64_t result = 0;
    for (const auto& arg: args) {
        auto num = dynamic_cast<MalInt*>(arg);
        if (!num){
            throw typeError("wrong type");
        }
        result -= num->get_elem();
    }
    return new MalInt(result);
}

MalType* operator_multiply(const std::vector<MalType *> &args) {
    int64_t result = 1;
    for (const auto& arg: args) {
        auto num = dynamic_cast<MalInt*>(arg);
        if (!num){
            throw typeError("wrong type");
        }
        result *= num->get_elem();
    }
    return new MalInt(result);
}

MalType* operator_divide(const std::vector<MalType *> &args) {
    if (args.empty()){
        throw typeError("empty arg list");
    }
    auto first = dynamic_cast<MalInt*>(args[0]);
    if (!first) {
        throw typeError("wrong type");
    }
    int64_t result = first->get_elem();
    for (std::size_t i = 1; i < args.size(); ++i) {
        auto arg = dynamic_cast<MalInt*>(args[i]);
        if (!arg) {
            throw typeError("wrong type");
        }
        int64_t num = arg->get_elem();
        if (num == 0){
            throw valueError("divided by zero");
        }
        result /= num;
    }

    return new MalInt(result);
}
