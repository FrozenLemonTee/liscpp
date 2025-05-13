#include "error.h"

syntaxError::syntaxError(const std::string &arg)
    : runtime_error(arg) {}

typeError::typeError(const std::string &arg)
    : runtime_error(arg) {}

valueError::valueError(const std::string &arg)
    : runtime_error(arg) {}

argInvalidError::argInvalidError(const std::string &arg)
    : runtime_error(arg) {}

IOError::IOError(const std::string &arg)
    : runtime_error(arg) {}

REPLError::REPLError(const std::string &arg)
    : runtime_error(arg) {}
