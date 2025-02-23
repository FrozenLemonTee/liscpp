#ifndef ERROR_H
#define ERROR_H
#include <stdexcept>

class syntaxError : public std::runtime_error {
    std::string msg;
public:
    explicit syntaxError(const std::string& arg);
};

inline syntaxError::syntaxError(const std::string &arg)
    : runtime_error(arg) {}

#endif //ERROR_H
