#include "printer.h"

std::string Printer::pr_str(MalType *mal_type) {
    return mal_type->to_string();
}
