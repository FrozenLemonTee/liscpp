#include <iostream>
#include "builtin.h"
#include <sstream>
#include <fstream>
#include "reader.h"
#include "printer.h"
#include "error.h"
#include "evaluator.h"


MalType* operator_plus(const std::vector<MalType *> &args) {
    int64_t result = 0;
    for (const auto& arg: args) {
        const auto num = dynamic_cast<MalInt*>(arg);
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
    const auto first = dynamic_cast<MalInt*>(args[0]);
    if (!first) {
        throw argInvalidError("wrong type");
    }
    int64_t result = first->get_elem();
    for (std::size_t i = 1; i < args.size(); ++i) {
        const auto arg = dynamic_cast<MalInt*>(args[i]);
        if (!arg) {
            throw argInvalidError("wrong type");
        }
        const int64_t num = arg->get_elem();
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
        const auto num = dynamic_cast<MalInt*>(arg);
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
    const auto first = dynamic_cast<MalInt*>(args[0]);
    if (!first) {
        throw argInvalidError("wrong type");
    }
    int64_t result = first->get_elem();
    for (std::size_t i = 1; i < args.size(); ++i) {
        const auto arg = dynamic_cast<MalInt*>(args[i]);
        if (!arg) {
            throw argInvalidError("wrong type");
        }
        const int64_t num = arg->get_elem();
        if (num == 0){
            throw valueError("divided by zero");
        }
        result /= num;
    }

    return new MalInt(result);
}

MalType* str(const std::vector<MalType *>& args) {
    const auto args_str = print_helper(args, false);
    std::stringstream ss;
    for (const auto& s: args_str){
        ss << s;
    }
    return new MalString(ss.str());
}

MalType* pr_str(const std::vector<MalType *>& args) {
    const auto args_str = print_helper(args, true);
    std::stringstream ss;
    bool first = true;
    ss << "\"";
    for (const auto& s: args_str){
        if (!first) {
            ss << " ";
        }
        ss << s;
        first = false;
    }
    ss << "\"";
    return new MalString(ss.str());
}

MalType* prn(const std::vector<MalType*>& args) {
    const auto args_str = print_helper(args, true);
    std::stringstream ss;
    bool first = true;
    for (const auto& s: args_str){
        if (!first) {
            ss << " ";
        }
        ss << s;
        first = false;
    }
    std::cout << ss.str();
    return new MalNil;
}

MalType* println(const std::vector<MalType *>& args) {
    const auto args_str = print_helper(args, false);
    std::stringstream ss;
    bool first = true;
    for (const auto& s: args_str){
        if (!first) {
            ss << " ";
        }
        ss << s;
        first = false;
    }
    ss << "\n";
    std::cout << ss.str();
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

    const auto arg = dynamic_cast<MalSequence*>(args[0]);
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
    auto sequence = dynamic_cast<MalSequence*>(args[0]);
    if (!sequence){
        throw argInvalidError("wrong type");
    }
    return new MalInt(static_cast<int64_t>(sequence->get_elem().size()));
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

MalType* not_func(const std::vector<MalType*>& args) {
    if (args.size() != 1) {
        throw argInvalidError("expected 1 arg, given " +
                              std::to_string(args.size()) + " arg(s)");
    }

    MalType* arg = args[0];
    const bool truthy = (!dynamic_cast<MalBool*>(arg) || dynamic_cast<MalBool*>(arg)->get_elem())
                  && !dynamic_cast<MalNil*>(arg);
    return new MalBool(!truthy);
}

MalType* compare_ints(const std::vector<MalType *> &args, const std::function<bool(int64_t, int64_t)>& cmp) {
    if (args.size() != 2) {
        throw argInvalidError("expected 2 args, given " +
                              std::to_string(args.size()) + " arg(s)");
    }
    const auto lhs = dynamic_cast<MalInt*>(args[0]);
    const auto rhs = dynamic_cast<MalInt*>(args[1]);
    if (!lhs || !rhs) {
        throw argInvalidError("wrong type");
    }
    return new MalBool(cmp(lhs->get_elem(), rhs->get_elem()));
}

std::vector<std::string> print_helper(const std::vector<MalType*>& args, const bool print_readably)
{
    std::vector<std::string> res;
    for (const auto& arg: args)
    {
        res.emplace_back(Printer::pr_str(arg, print_readably));
    }
    return res;
}

MalType* read_string(const std::vector<MalType*>& args) {
    if (args.size() != 1) {
        throw argInvalidError("expected 1 arg, given " +
                              std::to_string(args.size()) + " arg(s)");
    }
    auto str = dynamic_cast<const MalString*>(args[0]);
    if (!str){
        throw argInvalidError("wrong type");
    }
    return Reader::read_str(Reader::remove_comments(str->to_string(false)));
}

MalType* slurp(const std::vector<MalType*>& args) {
    if (args.size() != 1) {
        throw argInvalidError("expected 1 arg, given " +
                              std::to_string(args.size()) + " arg(s)");
    }
    auto str = dynamic_cast<const MalString*>(args[0]);
    if (!str){
        throw argInvalidError("wrong type");
    }
    std::ifstream ifs(str->to_string(false));
    if (!ifs){
        throw IOError("Can not open file: " + str->to_string(false));
    }
    std::stringstream ss;
    ss << ifs.rdbuf();
    return new MalString(ss.str());
}

MalType* evals(const std::vector<MalType *>& args) {
    if (args.size() != 1) {
        throw argInvalidError("expected 1 arg, given " +
                              std::to_string(args.size()) + " arg(s)");
    }
    return Evaluator::eval(args[0]);
}

MalType* load_file_repl(const std::vector<MalType*>& args){
    return load_file(args, true);
}

MalType* load_file(const std::vector<MalType*>& args, bool repl_mode) {
    auto file = slurp(args);
    auto str = dynamic_cast<MalString*>(file);
    if (!str) throw argInvalidError("slurp did not return string");

    const std::string no_comment = Reader::remove_comments(str->get_elem());

    const std::string wrapped = repl_mode ? "(do " + no_comment + "\n nil)" : no_comment;

    std::vector<MalType*> wrapped_args{ new MalString(wrapped) };
    auto ast = read_string(wrapped_args);
    return evals({ast});
}

MalType* atom(const std::vector<MalType*>& args) {
    if (args.size() != 1) {
        throw argInvalidError("expected 1 arg, given " +
                              std::to_string(args.size()) + " arg(s)");
    }
    return new MalRef(args[0]);
}

MalType* is_atom(const std::vector<MalType*>& args) {
    if (args.size() != 1) {
        throw argInvalidError("expected 1 arg, given " +
                              std::to_string(args.size()) + " arg(s)");
    }
    return new MalBool(dynamic_cast<const MalRef*>(args[0]));
}

MalType* deref(const std::vector<MalType*>& args) {
    if (args.size() != 1) {
        throw argInvalidError("expected 1 arg, given " +
                              std::to_string(args.size()) + " arg(s)");
    }
    const auto ref = dynamic_cast<const MalRef*>(args[0]);
    if (!ref){
        throw argInvalidError("wrong type");
    }
    return ref->get();
}

MalType* reset(const std::vector<MalType*>& args) {
    if (args.size() != 2) {
        throw argInvalidError("expected 2 args, given " +
                              std::to_string(args.size()) + " arg(s)");
    }
    auto ref = dynamic_cast<MalRef*>(args[0]);
    if (!ref){
        throw argInvalidError("wrong type");
    }
    ref->set(args[1]);
    return args[1];
}

MalType* swap(const std::vector<MalType*>& args) {
    if (args.size() < 2) {
        throw argInvalidError("expected at least 2 args, given " +
                              std::to_string(args.size()) + " arg(s)");
    }

    auto ref = dynamic_cast<MalRef*>(args[0]);
    auto fn = dynamic_cast<MalFunction*>(args[1]);
    if (!ref || !fn) {
        throw argInvalidError("wrong type");
    }

    std::vector<MalType*> fn_args;
    fn_args.push_back(ref->get());
    for (size_t i = 2; i < args.size(); ++i) {
        fn_args.push_back(args[i]);
    }

    MalType* result = fn->apply(fn_args);
    ref->set(result);
    return result;
}
