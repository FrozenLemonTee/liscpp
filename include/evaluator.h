#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "types.h"

class Evaluator {
    static Env* repl_env;
public:
    static MalType* eval(MalType* input, Env* env);
    static MalType* eval(MalType* input);
    static void set_env(Env* env);
};

#endif //EVALUATOR_H
