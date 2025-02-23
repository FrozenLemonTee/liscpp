#ifndef PRINTER_H
#define PRINTER_H
#include <string>

#include "types.h"

class Printer {
public:
    static std::string pr_str(MalType* mal_type);
};

#endif //PRINTER_H
