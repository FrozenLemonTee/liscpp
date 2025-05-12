#include "printer.h"

std::string Printer::pr_str(const MalType* const mal_object, const bool print_readably) {
    return mal_object->to_string(print_readably);
}
