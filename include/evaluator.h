#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "types.h"

class Evaluator {
public:
    static MalType* eval(MalType* input, Env* env);
};

#endif //EVALUATOR_H
