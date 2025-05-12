#ifndef PRINTER_H
#define PRINTER_H
#include <string>

#include "types.h"

class Printer {
public:
    static std::string pr_str(const MalType* mal_object, bool print_readably = true);
};

#endif //PRINTER_H
