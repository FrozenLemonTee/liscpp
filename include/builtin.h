#ifndef BUILTIN_H
#define BUILTIN_H

#include "types.h"

MalType* compare_ints(const std::vector<MalType*>& args,
                      const std::function<bool(int64_t, int64_t)>& cmp);
std::vector<std::string> print_helper(const std::vector<MalType*>& args, bool print_readably);


MalType* operator_plus(const std::vector<MalType*>& args);
MalType* operator_minus(const std::vector<MalType*>& args);
MalType* operator_multiply(const std::vector<MalType*>& args);
MalType* operator_divide(const std::vector<MalType*>& args);
MalType* str(const std::vector<MalType*>& args);
MalType* pr_str(const std::vector<MalType*>& args);
MalType* prn(const std::vector<MalType*>& args);
MalType* println(const std::vector<MalType*>& args);
MalType* list(const std::vector<MalType*>& args);
MalType* is_list(const std::vector<MalType*>& args);
MalType* is_empty(const std::vector<MalType*>& args);
MalType* count(const std::vector<MalType*>& args);
MalType* equal(const std::vector<MalType*>& args);
MalType* less(const std::vector<MalType*>& args);
MalType* less_equal(const std::vector<MalType*>& args);
MalType* greater(const std::vector<MalType*>& args);
MalType* greater_equal(const std::vector<MalType*>& args);
MalType* not_func(const std::vector<MalType*>& args);
MalType* read_string(const std::vector<MalType*>& args);
MalType* slurp(const std::vector<MalType*>& args);
MalType* evals(const std::vector<MalType*>& args);
MalType* load_file(const std::vector<MalType*>& args);
MalType* atom(const std::vector<MalType*>& args);
MalType* is_atom(const std::vector<MalType*>& args);
MalType* deref(const std::vector<MalType*>& args);
MalType* reset(const std::vector<MalType*>& args);
MalType* swap(const std::vector<MalType*>& args);


#endif //BUILTIN_H
