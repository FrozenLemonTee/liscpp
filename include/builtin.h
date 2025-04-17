#ifndef BUILTIN_H
#define BUILTIN_H

#include "types.h"


MalType* operator_plus(const std::vector<MalType*>& args);
MalType* operator_minus(const std::vector<MalType*>& args);
MalType* operator_multiply(const std::vector<MalType*>& args);
MalType* operator_divide(const std::vector<MalType*>& args);


#endif //BUILTIN_H
