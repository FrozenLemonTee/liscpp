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
        throw argInvalidError("expected 1 arg, given " +
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
        throw argInvalidError("expected 1 arg, given " +
                              std::to_string(args.size()) + " arg(s)");
    }
    if (dynamic_cast<MalNil*>(args[0])){
        return new MalInt(0);
    }
    auto arg = dynamic_cast<MalList*>(args[0]);
    if (!arg){
        throw argInvalidError("wrong type");
    }
    return new MalInt(static_cast<int64_t>(arg->get_elem().size()));
}

MalType* equal(const std::vector<MalType *> &args) {
    if (args.size() != 2){
        throw argInvalidError("expected 2 args, given " +
                              std::to_string(args.size()) + " arg(s)");
    }
    return new MalBool(args[0]->equal(args[1]));
}

MalType* less(const std::vector<MalType*>& args) {
    return compare_ints(args, std::less<int64_t>{});
}

MalType* less_equal(const std::vector<MalType*>& args) {
    return compare_ints(args, std::less_equal<int64_t>{});
}

MalType* greater(const std::vector<MalType*>& args) {
    return compare_ints(args, std::greater<int64_t>{});
}

MalType* greater_equal(const std::vector<MalType*>& args) {
    return compare_ints(args, std::greater_equal<int64_t>{});
}

MalType* compare_ints(const std::vector<MalType *> &args, const std::function<bool(int64_t, int64_t)>& cmp) {
    if (args.size() != 2) {
        throw argInvalidError("expected 2 args, given " +
                              std::to_string(args.size()) + " arg(s)");
    }
    auto lhs = dynamic_cast<MalInt*>(args[0]);
    auto rhs = dynamic_cast<MalInt*>(args[1]);
    if (!lhs || !rhs) {
        throw argInvalidError("wrong type");
    }
    return new MalBool(cmp(lhs->get_elem(), rhs->get_elem()));
}
