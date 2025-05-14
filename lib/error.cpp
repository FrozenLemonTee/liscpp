#include "error.h"

liscppError::liscppError(const std::string &arg)
    : runtime_error(arg) {}

syntaxError::syntaxError(const std::string &arg)
    : liscppError(arg) {}

typeError::typeError(const std::string &arg)
    : liscppError(arg) {}

valueError::valueError(const std::string &arg)
    : liscppError(arg) {}

argInvalidError::argInvalidError(const std::string &arg)
    : liscppError(arg) {}

IOError::IOError(const std::string &arg)
    : liscppError(arg) {}

REPLError::REPLError(const std::string &arg)
    : liscppError(arg) {}
