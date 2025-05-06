#include "printer.h"

std::string Printer::pr_str(const MalType* const mal_object) {
    return mal_object->to_string();
}
